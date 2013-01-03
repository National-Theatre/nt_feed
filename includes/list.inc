<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

class NtFeedList {
  
  protected $moduleClasses;
  protected $moduleIntClassList = array();
  protected $transport;
  protected $currentOptions;
  protected $hiddenList = array();
  protected $itemList = array();
  protected $itemData = array();

  public function __construct($transport) {
    $this->transport = $transport;
    $mod_list = variable_get('nt_feed_data_types', array());
    $this->moduleClasses = array_keys($mod_list);
  }
  
  public function setup($options, $reset = TRUE) {
    $this->currentOptions = $options;
    if ($reset) {
      $this->moduleIntClassList = array();
    }
    foreach ($this->moduleClasses as $className) {
      if (isset($options[$className]) && !empty($options[$className])) {
        if (isset($options[$className][0])
          && is_array($options[$className][0])) {
          foreach ($options[$className] as $key => $value) {
            if (is_numeric($key)) {
              $value += array(
                'filter'    => '',
                'start'     => NULL,
                'number'    => NULL,
                'transport' => 'default',
              );
              $options[$className][$key] += array(
                'filter'    => '',
                'start'     => NULL,
                'number'    => NULL,
                'transport' => 'default',
              );
              $obj = new $className(
                $this->transport[$options[$className][$key]['transport']],
                $value
              );
              $this->moduleIntClassList[$obj->getId()] = $obj;
            }
          }
        }
        else {
          $options[$className] += array(
            'filter'    => '',
            'start'     => NULL,
            'number'    => NULL,
            'transport' => 'default',
          );
          $obj = new $className(
            $this->transport[$options[$className]['transport']],
            $value
          );
          $this->moduleIntClassList[$obj->getId()] = $obj;
        }
      }
      else {
        $obj = new $className($this->transport['default']);
        $this->moduleIntClassList[$obj->getId()] = $obj;
      }
    }
  }
  
  public function getConfig() {
    return $this->currentOptions;
  }
  
  public function refresh($type = FALSE) {
    if ($type !== FALSE) {
      foreach ($this->moduleIntClassList as $obj) {
        if (get_class($obj) == $type) {
          $obj = new $type(
            $this->transport,
            $this->currentOptions[$type]
          );
        }
      }
    }
    else {
      $this->setup($this->currentOptions);
    }
  }
  
  public function buildList() {
    $output = array(
      'content' => array(
        '#type' => 'container',
        '#tree' => TRUE,
      ),
      '#theme' => 'nt_feed_list_wrapper',
      '#tree'  => TRUE,
    );
    foreach ($this->moduleIntClassList as $obj_key => $obj) {
      $types = variable_get('nt_feed_data_types', array());
      $settings = $types[get_class($obj)];
      $mod_output = array(
        '#title'   => $obj->getTitle(),
        '#css_id'  => $obj->getId(),
        'content' => array(),
        '#theme'   => 'nt_feed_list_item_wrapper',
        '#tree'    => TRUE,
      );
      $counter = 1;
      $obj->rewind();
      $is_more = FALSE;
      foreach ($obj as $item) {
        // Add default values so we have no errors.
        $item += array(
          'raw_data' => 'N/A',
          'title' => 'N/A',
          'date' => 'N/A',
          'id' => 'N/A',
          'html_id' => 'N/A',
          'description' => 'N/A',
          'hidden' => FALSE,
        );
        if (isset($this->hiddenList[$obj->getId()])
          && array_key_exists($item['id'], $this->hiddenList[$obj->getId()])) {
          $item['hidden'] = TRUE;
        }
//        print '<pre>' . print_r(get_class($obj), TRUE) . '</pre>';
//        print '<pre>' . print_r($item, TRUE) . '</pre>';
        $mod_output['content'][] = array(
          '#data'  => $item,
          'add' => array(
            '#name' => $item['html_id'] . '_sub',
            '#type' => 'submit',
            '#value'  => t('Add'),
            '#submit' => array('nt_feed_buildlist_add_submit'),
            '#limit_validation_errors' => array(),
            '#data' => array(
              'id' => $item['id'],
              'type' => $obj->getId(),
              'item' => $item,
            ),
          ),
          '#theme' => 'nt_feed_list_item',
        );
        if ($counter > $obj->getNumberToReturn()) {
          $is_more = TRUE;
          break;
        }
        if ($item['hidden'] !== TRUE) {
          $counter++;
        }
      }
      if ($is_more) {
        $mod_output['more'] = array(
          '#name' => $obj->getId() . '_more',
          '#type'   => 'submit',
          '#value'  => t('More'),
          '#submit' => array('nt_feed_buildlist_more_submit'),
          '#limit_validation_errors' => array(),
          '#data' => array(
            'type'    => $obj->getId(),
            'counter' => $obj->getNumberToReturn(),
            'obj'     => $obj,
            'key'     => $obj_key,
          ),
          '#ajax' => array(
            'callback' => 'nt_feed_buildlist_more_submit_callback',
            'wrapper' => $obj->getId(),
            'effect' => 'fade',
          ),
        );
//        print '<pre>' . print_r($obj->getNumberToReturn(), TRUE) . '</pre>';
      }
      else {
        $mod_output['more'] = FALSE;
      }

      $output[$obj->getId()] = $mod_output;
    }
    return $output;
  }
  
  public function hide($type, $id) {
    $this->hiddenList[$type][$id] = TRUE;
  }
  
  public function unhide($type, $id) {
    unset($this->hiddenList[$type][$id]);
  }
  
  public function addItem($type, $id, $weight = 0) {
    $this->itemList[$type][$id] = $weight;
    $this->itemData[$type][$id] = array();
  }
  
  public function addItemData($type, $id, $data) {
    $this->itemData[$type][$id] = $data;
  }
  
  public function delItem($type, $id) {
    unset($this->itemList[$type][$id]);
    unset($this->itemData[$type][$id]);
//    print '<pre>' . print_r($this->itemList, TRUE) . '</pre>';
  }
  
  public function getItems() {
    return $this->itemList;
  }
  
  public function getItemsData() {
    return $this->itemData;
  }
  
  /**
   * Get an item in feed.
   *
   * @param string $type
   *   The type to search through.
   * @param string $id
   *   The id to search for.
   *
   * @return Mixed
   *   False if it can't be found, or array of data.
   */
  public function getItem($type, $id) {
    if (isset($this->itemList[$type][$id])) {
      return $this->itemList[$type][$id];
    }
    else {
      return FALSE;
    }
  }
  /**
   * Get an item data in feed.
   *
   * @param string $type
   *   The type to search through.
   * @param string $id
   *   The id to search for.
   *
   * @return Mixed
   *   False if it can't be found, or array of data.
   */
  public function getItemData($type, $id) {
    if (isset($this->itemData[$type][$id])) {
      return $this->itemData[$type][$id];
    }
    else {
      return FALSE;
    }
  }
  
  public function renderItems($form_id = '') {
    $output = array();
//    print '<pre>' . print_r($this->itemList, TRUE) . '</pre>';
//    die('<pre>' . print_r($this->itemList, TRUE) . '</pre>');
    foreach ($this->moduleIntClassList as $obj) {
      if (isset($this->itemList[$obj->getId()])) {
        foreach (array_keys($this->itemList[$obj->getId()]) as $id) {
          $item_array = $obj->render($id);
          $form_details = $obj->getId() . '/' . $id . '/' . $form_id;
          $item_array['data']['tools'] = array(
            '#theme' => 'links__ctools_dropbutton',
            '#links' => array(
              'override-title' => array(
                'title' => t('Override title'),
                'href' => "ahah/inspire-me-feed/nojs/title/" . $form_details,
                'attributes' => array('class' => array('nt-feed-ajax-link', 'ctools-use-modal')),
              ),
              'cover-image' => array(
                'title' => t('Cover image'),
                'href' => "ahah/inspire-me-feed/nojs/image/" . $form_details,
                'attributes' => array('class' => array('nt-feed-ajax-link', 'ctools-use-modal')),
              ),
              'schedule' => array(
                'title' => t('Schedule'),
                'href' => "ahah/inspire-me-feed/schedule/" . $form_id,
                'attributes' => array('class' => array('nt-feed-ajax-link', 'ctools-use-modal')),
              ),
            ),
          );
          $item_array['#weight'] = $this->itemList[$obj->getId()][$id];
          $item_array['data']['weight']['#default_value']
           = $this->itemList[$obj->getId()][$id];
          $output[] = $item_array;
        }
      }
    }
    return $output;
  }
}

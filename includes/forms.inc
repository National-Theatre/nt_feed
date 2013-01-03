<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * @param array $form
 *   Nested array of form elements that comprise the form.
 * @param array $form_state
 *   A keyed array containing the current state of the form.
 * @param string $form_id
 *   String representing the name of the form itself.
 */
function nt_feed_form_ctools_export_ui_edit_item_form_alter(&$form, &$form_state, $form_id) {
  nt_feed_base_form_alter($form, $form_state, $form_id);
}
function nt_feed_base_form_alter(&$form, &$form_state, $form_id) {
  $data_list = $form['#data_list'];
  $form['schedular']['plane'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme' => 'nt_feed_form_plane',
    'table' => array(
      '#tree' => TRUE,
      '#theme' => 'nt_feed_item_table',
      'rows' => $data_list->renderItems($form['#build_id']),
    ),
  );
}

function nt_feed_form(&$form, &$form_state) {
  ctools_include('ajax');
  drupal_add_library('system', 'drupal.ajax');
  ctools_include('dropbutton.theme');
  ctools_include('modal');
  ctools_modal_add_js();
  $form['#return_path'] = arg();
  if (substr($form['#return_path'][count($form['#return_path'])-1], 0, 5) == 'form-') {
    $form_id = $form['#return_path'][count($form['#return_path'])-1];
    $form = form_get_cache($form_id, $form_state);
    $form['#return_path'] = arg();
    unset($form['#return_path'][count($form['#return_path'])-1]);
  }
  if (isset($form['#data_list'])) {
    // Something.
    // Populate missing data.
  }
  elseif (!isset($form_state['complete form']['#data_list'])) {
    $options = array();
    drupal_alter('nt_feed_register_settings_defaults', $options);
    if (isset($form_state['item']->config) && is_array($form_state['item']->config)) {
      $options = $form_state['item']->config + $options;
    }
    $transport = array(
      'default' => new \NtFeedTransport(),
      'db'      => 'Use drupal db',
    );

    $data_list = new \NtFeedList($transport);
    $data_list->setup($options);
    $form['#data_list'] = $data_list;
    if (isset($form_state['item']->items) && is_array($form_state['item']->items)) {
      foreach ($form_state['item']->items as $item) {
        $type  = $item['data']['type'];
        $id    = $item['data']['id'];
        $order = $item['data']['weight'];
        $data_list->hide($type, $id);
        $data_list->addItem($type, $id, $order);
        $data = $item['data'];
        unset($data['type']);
        unset($data['id']);
        unset($data['weight']);
        $data_list->addItemData($type, $id, $data);
      }
    }
  }
  else {
    $data_list = $form_state['complete form']['#data_list'];
    $form['#data_list'] = $data_list;
  }

  $form['#attached']['library'][] = array(
    'system',
    'ui.draggable',
  );
  $form['#attached']['library'][] = array(
    'system',
    'ui.droppable',
  );
  $form['#attached']['library'][] = array(
    'system',
    'ui.sortable',
  );
  
  $form['#attached']['css'][]
   = drupal_get_path('module', 'nt_feed') . '/css/nt_feed.css';
  
  $form['#attached']['js'][]
   = drupal_get_path('module', 'nt_feed') . '/js/nt_feed.js';

  $form['#base'] = 'nt_feed_form';
  //$form['#theme'] = 'nt_feed_form';
  
  $form['info']['name']['#element_validate'] = array('nt_feed_name_validate');
  
  $form['info']['tag'] = array(
    '#title'     => t('Tag'),
    '#type'      => 'textfield',
    '#required'  => FALSE,
    '#maxlength' => 255,
    '#default_value'   => isset($form_state['item'])? $form_state['item']->tag : '',
    '#description' => t('Example: <code>Production</code>') .'<br/>'. t('A tag to group this feed with others.'),
  );

  $form['info']['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#maxlength' => 255,
    '#default_value'   => isset($form_state['item'])? $form_state['item']->description : '',
    '#description' => t('The description of this feed definition.'),
  );
  
  $form['feed-data'] = array(
    '#type' => 'hidden',
  );
  
  $form['tabs'] = array(
    '#type'   => 'vertical_tabs',
    '#prefix' => '<div id="nt-feed-tabs">',
    '#suffix' => '</div>',
  );
  
  $form['schedular'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feed'),
    '#collapsible' => TRUE,
    '#group' => 'tabs',
    '#tree' => TRUE,
    '#theme' => 'nt_feed_form_panel',
  );
  
  drupal_add_tabledrag('item-list-table', 'order', 'group', 'nt-feed-item-order');
  
  $form['schedular']['plane'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme' => 'nt_feed_form_plane',
    'table' => array(
      '#tree' => TRUE,
      '#theme' => 'nt_feed_item_table',
      //'rows' => $data_list->renderItems(),
    ),
  );
  
  $form['schedular']['list'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme' => 'nt_feed_form_list',
    'list' => $form['#data_list']->buildList(),
  );
  $form['config_tab'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Config'),
    '#collapsible' => TRUE,
    '#group'       => 'tabs',
    '#tree'        => TRUE,
  );
  
  drupal_alter('nt_feed_register_settings_form', $form['config_tab'], $form_state);
  $form['config_tab']['update'] = array(
    '#type' => 'submit',
    '#value' => t('Save Config'),
    '#weight' => count($form['config_tab']),
    '#submit' => array('nt_feed_settings_submit'),
    '#limit_validation_errors' => array(),
  );
  
//  $form['actions']['save'] = array(
//    '#type' => 'submit',
//    '#value' => t('Save'),
//  );
//  print '<pre>' . print_r($form['schedular']['plane'], TRUE) . '</pre>';
//  return $form;
//  die('<pre>' . print_r($form['schedular']['list'], TRUE) . '</pre>');
}

function nt_feed_form_submit($form, &$form_state) {
//  die('hello!<pre>' . print_r($form_state['triggering_element'], TRUE) . '</pre>');
//  $parse_data = \NtFeed::parseList($form_state);
//  if (!empty($parse_data)) {
//    if (nt_feed_save($parse_data)) {
//      drupal_set_message(t('Feed Saved.'));
//    }
//    else {
//      drupal_set_message(t('Did not save feed.'), 'warning');
//    }
//  }
//  else {
//    drupal_set_message(t('No data to save.'), 'warning');
//  }
  $form_state['rebuild'] = TRUE;
}

function nt_feed_settings_submit($form, &$form_state) {
  $settings = array();
  foreach ($form_state['complete form']['config_tab'] as $key => $item) {
    if (substr($key, 0, 1) != '#') {
      $settings[$key] = array();
      foreach ($item as $name => $value) {
        if (substr($name, 0, 1) != '#') {
          if (is_numeric($name)) {
            foreach ($value as $key1 => $value1) {
              if (substr($key1, 0, 1) != '#' && strlen(trim($value1['#value'])) > 0) {
                $settings[$key][$name][$key1] = $value1['#value'];
              }
            }
          }
          elseif (!in_array(strtolower($name), array('more', 'add'))
            && strlen(trim($value['#value'])) > 0) {
            $settings[$key][$name] = $value['#value'];
          }
        }
      }
    }
  }
  $form_state['complete form']['#data_list']->setup($settings);
  $form_state['rebuild'] = TRUE;
}

function nt_feed_buildlist_more_submit_callback($form, &$form_state) {
  //die('hello!<pre>' . print_r($form_state['triggering_element'], TRUE) . '</pre>');
  $key = $form_state['triggering_element']['#data']['key'];
  return $form['schedular']['list']['list'][$key];
}

function nt_feed_buildlist_more_submit($form, &$form_state) {
  $form_state['triggering_element']['#data']['obj']->incNumberToReturn(10);
//  die('<pre>' . print_r($form_state, TRUE) . '</pre>');
  $form_state['rebuild'] = TRUE;
}

function nt_feed_buildlist_add_submit($form, &$form_state) {
//  print '<pre>' . print_r($form_state['triggering_element']['#data'], TRUE) . '</pre>';
//  print '<pre>' . print_r($form_state['complete form']['#data_list'], TRUE) . '</pre>';
  $data_list = $form_state['complete form']['#data_list'];
  $type = $form_state['triggering_element']['#data']['type'];
  $id = $form_state['triggering_element']['#data']['id'];
  $data_list->hide($type, $id);
  $data_list->addItem($type, $id);
//  die('<pre>' . print_r($form_state, TRUE) . '</pre>');
  $form_state['rebuild'] = TRUE;
}

function nt_feed_buildlist_del_submit($form, &$form_state) {
//  print '<pre>del</pre>';
  $data_list = $form_state['complete form']['#data_list'];
  $type      = $form_state['triggering_element']['#data']['type'];
  $id        = $form_state['triggering_element']['#data']['id'];
  $data_list->unhide($type, $id);
  $data_list->delItem($type, $id);
  $form_state['rebuild'] = TRUE;
}
/**
 * Replacement for ctools_export_ui_edit_name_validate(). Allow dashes.
 */
function nt_feed_name_validate($element, &$form_state) {
  $plugin = $form_state['plugin'];
  // Check for string identifier sanity
  if (!preg_match('!^[a-z0-9_-]+$!', $element['#value'])) {
    form_error($element, t('The name can only consist of lowercase letters, underscores, dashes, and numbers.'));
    return;
  }

  // Check for name collision
  if ($form_state['op'] != 'edit') {
    if (empty($form_state['item']->export_ui_allow_overwrite) && $exists = ctools_export_crud_load($plugin['schema'], $element['#value'])) {
      form_error($element, t('A @plugin with this name already exists. Please choose another name or delete the existing item before creating a new one.', array('@plugin' => $plugin['title singular'])));
    }
  }
}

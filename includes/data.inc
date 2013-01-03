<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

abstract class NtFeedData implements Iterator {

  protected $transport;
  protected $data;
  protected $pointer = 0;
  protected $numberToReturn = 10;
  protected $label;
  protected $id;
  protected $title;
  protected $options;
  protected $lastPointer;

  abstract public function __construct($transport, $options);

  public function getLabel() {
    return $this->label;
  }

  public function getId() {
    return $this->id;
  }

  public function getTitle() {
    return $this->title;
  }

  public function get($number = 10) {
    $format = $this->url;
  }
  abstract public function paginate($number = 10);
  abstract public function filter($filter);
  public function register() {
    return array(
      'class' => self::className(),
      'path' => self::path(),
    );
  }
  public function current() {
    return $this->data[$this->pointer];
  }

  public function key() {
    return $this->pointer;
  }

  public function next() {
    $this->pointer++;
  }

  public function rewind() {
    $this->pointer = 0;
  }
  public function getNumberToReturn() {
    return $this->numberToReturn;
  }
  public function incNumberToReturn($val) {
    $this->numberToReturn += $val;
  }
  
  public function render($id) {
    if (!isset($this->dataKey[$id])) {
      $this->getItem($id);
    }
    $output = array(
      '#theme' => 'nt_feed_item_row',
      'data' => array(
        'weight' => array(
          '#type' => 'weight',
          '#delta' => 100,
          '#attributes' => array(
            'class' => array('nt-feed-item-order'),
          ),
        ),
        'id' => array(
          '#type' => 'hidden',
          '#value' => $id,
        ),
        'type' => array(
          '#type' => 'hidden',
          '#value' => __CLASS__,
        ),
        'feed_type' => array(
          '#type' => 'hidden',
          '#value' => __CLASS__,
        ),
        'label' => array(
          '#type' => 'item',
          '#markup' => t(__CLASS__),
        ),
        'del' => array(
          '#name' => __CLASS__ . '_' . $id . '_del',
          '#type' => 'submit',
          '#value'  => t('Remove'),
          '#submit' => array('nt_feed_buildlist_del_submit'),
          '#limit_validation_errors' => array(),
          '#data' => array(
            'id' => $id,
            'type' => __CLASS__,
          ),
        ),
      ),
    );
    return $output;
  }
  
  abstract public function getItem($id);
}

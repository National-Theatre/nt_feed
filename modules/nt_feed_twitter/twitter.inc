<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

class NtFeedTwitter extends NtFeedData {
  
  protected $search   = "http://search.twitter.com/search.json?result_type=recent&rpp=100&q=";
  protected $timeline = "https://api.twitter.com/1/statuses/user_timeline.json?count=200&screen_name=";
  protected $tweet    = 'http://api.twitter.com/1/statuses/show/|id|.json';

  public function __construct($transport, $options) {
    $this->transport = $transport;
    $this->options = $options;
    $this->label = trim($options['screen_name']);
    $this->title = 'Twitter - ' . trim($options['screen_name']);
    $this->id = md5(
      __CLASS__ . trim($options['screen_name'])
    );
    if (strlen(trim($options['filter'])) > 0) {
      $query = '';
      if (isset($options['screen_name']) && strlen(trim($options['screen_name'])) > 0) {
        $query .= 'from:' . $options['screen_name'] . ' ';
      }
      $query .= '#' . $options['filter'];
      $this->url = $this->search . urlencode($query) . '&src=hash';
    }
    else {
      $this->url = $this->timeline . urlencode(trim($options['screen_name']));
    }
    
    $this->getItems($this->url);
  }

  public function filter($filter) {
    
  }

  public function paginate($number = 10) {
    
  }

  public function valid() {
    if (!isset($this->data[$this->pointer])) {
      $url = $this->url;
      if (count($this->data) > 0) {
        $url .= '&max_id=' . $this->lastPointer;
      }
      $this->getItems($url);
      if (!isset($this->data[$this->pointer])) {
        return FALSE;
      }
    }
    return TRUE;
  }

  protected function getItems($url) {
    $result = $this->transport->get(
      array(
        'url' => $url,
        'options' => array(
          'method' => 'GET',
        ),
      )
    );
    if ($result->code == 200) {
      $data = json_decode($result->data);
      if (is_object($data) && isset($data->results)) {
        $data = $data->results;
      }
      foreach ($data as $item) {
        if ($item->id == $this->lastPointer) {
          continue;
        }
         $store = array(
          'raw_data' => $item,
          'title' => 'Tweet',
          'date' => strtotime($item->created_at),
          'id' => $item->id,
          'html_id' => 'tweet_' . $item->id,
          'description' => drupal_substr($item->text, 0, 50),
        );
         $this->dataKey[$item->id] = $store;
         $this->data[] = $this->dataKey[$item->id];
      }
      if (is_object($this->data[count($this->data)-1])) {
        $this->lastPointer = $this->data[count($this->data)-1]->id;
      }
      else {
        $this->lastPointer = $this->data[count($this->data)-1]['id'];
      }
      //die('<pre>' . print_r($result, TRUE) . '</pre>');
      return count($data) > 0;
    }
    else {
      // Log error message.
    }
    //die('<pre>' . print_r($result, TRUE) . '</pre>');
    return FALSE;
  }
  
  public function render($id) {
    $output = parent::render($id);
    $output['data']['type']['#value'] = $this->getId();
    $output['data']['feed_type']['#value'] = 'Twitter';
    $output['data']['label']['#markup'] = t($this->dataKey[$id]['description']);
    $output['data']['del']['#name'] = __CLASS__ . '_' . $id . '_del';
    $output['data']['del']['#data']['type'] = $this->getId();
    return $output;
  }
  
  public function getItem($id) {
    $url = str_replace('|id|', $id, $this->tweet);
    $result = $this->transport->get(
      array(
        'url' => $url,
        'options' => array(
          'method' => 'GET',
        ),
      )
    );
    if ($result->code == 200) {
      $data = json_decode($result->data);
      $store = array(
        'raw_data' => $data,
        'title' => 'Tweet',
        'date' => strtotime($data->created_at),
        'id' => $data->id,
        'html_id' => 'tweet_' . $data->id,
        'description' => drupal_substr($data->text, 0, 50),
      );
      $this->dataKey[$data->id] = $store;
    }
  }
}

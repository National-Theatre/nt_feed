<?php
/**
 * @file
 * Why have this object, so it can be mocked out sanely to get nice Unit tests.
 *
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

class NtFeedTransport {
  
  function get($variables) {

    $url     = $variables['url'];
    $options = $variables['options'];

    $result = drupal_http_request($url, $options);
    drupal_set_message('Doing get request');
    return $result;
  }
  
}

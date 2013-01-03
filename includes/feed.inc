<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

class NtFeed {
  
  public static function parseList($form) {
    $data_list = $form['complete form']['#data_list'];
    if (isset($form['values'])) {
      if (isset($form['values']['schedular']['plane']['table']['rows'])) {
        $data = $form['values']['schedular']['plane']['table']['rows'];
        foreach ($data as &$item) {
          $item['data'] = $data_list->getItemData(
            $item['data']['type'],
            $item['data']['id']
          ) + $item['data'];
        }
        return $data;
      }
    }
    return array();
  }
}

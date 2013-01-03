<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

class nt_feed_export_ui extends ctools_export_ui {
  
  /**
   * Override of edit_form_submit().
   * Don't copy values from $form_state['values'].
   */
//  function edit_form_submit(&$form, &$form_state) {
//    if (!empty($this->plugin['form']['submit'])) {
//      $this->plugin['form']['submit']($form, $form_state);
//    }
//  }
  
  function edit_execute_form_standard(&$form_state) {
    $output = drupal_build_form('ctools_export_ui_edit_item_form', $form_state);
    if (isset($form_state['triggering_element']['#submit'])) {
      $form_state['executed'] = FALSE;
    }
    if (!empty($form_state['executed'])) {
      $this->edit_save_form($form_state);
    }

    return $output;
  }

  function edit_save_form($form_state) {
    $plugin = $form_state['plugin'];
    $data_list = $form_state['complete form']['#data_list'];
//    if (!isset($form_state['triggering_element']['#submit'])) {
    $parse_data = \NtFeed::parseList($form_state);
    $data = array(
      'name'        => $form_state['values']['name'],
      'tag'         => $form_state['values']['tag'],
      'description' => $form_state['values']['description'],
      'items'       => $parse_data,
      'config'      => $data_list->getConfig(),
      'created'     => time(),
    );
    $exists = ctools_export_crud_load($plugin['schema'], $form_state['values']['name']);
    if ($exists) {
      unset($data['created']);
      $data['updated'] = time();
      drupal_write_record('nt_feed', $data, 'name');
      drupal_set_message(t('Feed updated.'));
    }
    else {
      drupal_write_record('nt_feed', $data);
      drupal_set_message(t('Feed Saved.'));
    }

    if (empty($parse_data)) {
      drupal_set_message(t('No data to save.'), 'warning');
    }
//    }
//    $item = &$form_state['item'];
//    $export_key = $this->plugin['export']['key'];
//
//    $result = ctools_export_crud_save($this->plugin['schema'], $item);
//
//    if ($result) {
//      $message = str_replace('%title', check_plain($item->{$export_key}), $this->plugin['strings']['confirmation'][$form_state['op']]['success']);
//      drupal_set_message($message);
//    }
//    else {
//      $message = str_replace('%title', check_plain($item->{$export_key}), $this->plugin['strings']['confirmation'][$form_state['op']]['fail']);
//      drupal_set_message($message, 'error');
//    }
  }
}

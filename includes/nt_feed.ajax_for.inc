<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

/**
 * Helper functions.
 */
/**
 * 
 * @param array $form
 * @param array $form_state
 * @param string $type
 * @param string $item_id
 */
function _nt_feed_form_start(&$form, $form_state, &$type, &$item_id) {
  if (!isset($form_state['complete form']['#data_list'])) {
    $form_build_id        = $form_state['build_info']['args'][3];
    $old_form_state       = form_state_defaults();
    $old_form             = form_get_cache($form_build_id, $old_form_state);
    $form['#data_list']   = $old_form['#data_list'];
    $form['#return_path'] = $old_form['#return_path'];
    $type                 = $form_state['build_info']['args'][2];
    $item_id              = $form_state['build_info']['args'][1];
    $form['#isjs']        = $form_state['build_info']['args'][0];
  }
  else {
    $form['#data_list']   = $form_state['complete form']['#data_list'];
    $form['#return_path'] = $form_state['complete form']['#return_path'];
    $form['#isjs']        = $form_state['complete form']['#isjs'];
    $type                 = $form_state['input']['old_type'];
    $item_id              = $form_state['input']['old_id'];
  }
  if (isset($form_state['#cancel'])) {
    $form['#cancel'] = $form_state['#cancel'];
  }
  if (isset($form_state['#done'])) {
    $form['#done'] = $form_state['#done'];
  }
}

function _nt_feed_form_end(&$form, &$form_state, &$type, &$item_id) {
  $form['old_build_id'] = array(
    '#type'  => 'hidden',
    '#value' => $form_state['build_info']['args'][3],
  );
  $form['old_type'] = array(
    '#type'  => 'hidden',
    '#value' => $type,
  );
  $form['old_id'] = array(
    '#type'  => 'hidden',
    '#value' => $item_id,
  );
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Update'),
    '#description' => t('You need to save this change for it to be perminant.'),
  );
  $form['cancel'] = array(
    '#type'   => 'submit',
    '#value'  => t('Cancel'),
    '#submit' => array('nt_feed_form_cancel'),
    '#limit_validation_errors' => array(),
  );
}

function _nt_feed_form_redirect(&$form_state) {
  if ($form_state['complete form']['#isjs']) {
    $form_state['#done'] = TRUE;
    $form_state['rebuild'] = TRUE;
  }
  else {
    $form_state['redirect']
      = implode('/', $form_state['complete form']['#return_path'])
      . '/' . $form_state['input']['old_build_id'];
  }
}
/**
 * Page Code.
 */
/**
 * 
 * @param string $form_id
 * @param boolean $js
 * @param string $type
 * @param string $item_id
 * @param string $build_id
 *
 * @return Mixed
 *   Html output or array of commands
 */
function nt_feed_form_page($form_id, $js, $type, $item_id, $build_id) {
  if ($js) {
    ctools_include('modal');
    ctools_include('ajax');
  }
  $form = drupal_get_form($form_id, $js, $item_id, $type, $build_id);
  $output = drupal_render($form);
  if ($js) {
    $commands = array();
    if (isset($form['#cancel']) && $form['#cancel']) {
      $commands[] = ctools_modal_command_dismiss();
    }
    elseif (isset($form['#done']) && $form['#done']) {
      $commands[] = ctools_modal_command_dismiss();
    }
    else {
      $commands = ctools_modal_form_render(array(), $output);
    }
    print ajax_render($commands);
    exit;
  }
  else {
    return $output;
  }
}
/**
 * Form code.
 */
/**
 * 
 * @param type $form
 * @param type $form_state
 * @return string
 */
function nt_feed_image_form($form, &$form_state) {
  $type = '';
  $item_id = '';
  _nt_feed_form_start($form, $form_state, $type, $item_id);

  $item_data = $form['#data_list']->getItemData($type, $item_id);
  if ($item_data && isset($item_data['cover_fid'])) {
    $cover_fid = $item_data['cover_fid'];
  }
  else {
    $cover_fid = '';
  }

  
  $form['cover-image'] = array(
    '#title' => t('Cover Image'),
    '#type' => 'managed_file',
    '#description' => t('The uploaded image will be displayed as the cover of this item.'),
    '#default_value' => $cover_fid,
    '#upload_location' => 'public://nt_feed/',
    '#required' => TRUE,
  );
  _nt_feed_form_end($form, $form_state, $type, $item_id);
  return $form;
}

function nt_feed_image_form_submit($form, &$form_state) {
  $form_build_id = $form_state['input']['old_build_id'];
  $old_form_state = form_state_defaults();
  $old_form = form_get_cache($form_build_id, $old_form_state);
  
  $item_data = $old_form['#data_list']->getItemData(
    $form_state['input']['old_type'],
    $form_state['input']['old_id']
  );

  $new_data = array('cover_fid' => $form_state['values']['cover-image']);
  if ($item_data) {
    $new_data += $item_data;
  }

  $old_form['#data_list']->addItemData(
    $form_state['input']['old_type'],
    $form_state['input']['old_id'],
    $new_data
  );
  
  form_set_cache($form_build_id, $old_form, $old_form_state);
  $file = file_load($form_state['values']['cover-image']);
  $file->status = FILE_STATUS_PERMANENT;
  file_save($file);
  global $user;
  file_usage_add($file, 'user', 'user', $user->uid);
  _nt_feed_form_redirect($form_state);
}

function nt_feed_form_cancel($form, &$form_state) {
  if ($form_state['complete form']['#isjs']) {
    $form_state['#cancel'] = TRUE;
    $form_state['rebuild'] = TRUE;
  }
  else {
    $form_state['redirect']
      = implode('/', $form_state['complete form']['#return_path'])
      . '/' . $form_state['input']['old_build_id'];
  }
}

function nt_feed_title_form($form, &$form_state) {
  $type = '';
  $item_id = '';
  _nt_feed_form_start($form, $form_state, $type, $item_id);

  $item_data = $form['#data_list']->getItemData($type, $item_id);
  if ($item_data && isset($item_data['override-title'])) {
    $title = $item_data['override-title'];
  }
  else {
    $title = '';
  }  
  $form['override-title'] = array(
    '#title' => t('Override Title'),
    '#type' => 'textfield',
    '#description' => t('The Overrride for this item title.'),
    '#default_value' => $title,
    '#required' => TRUE,
  );
  _nt_feed_form_end($form, $form_state, $type, $item_id);
  return $form;
}

function nt_feed_title_form_submit($form, &$form_state) {
  $form_build_id = $form_state['input']['old_build_id'];
  $old_form_state = form_state_defaults();
  $old_form = form_get_cache($form_build_id, $old_form_state);
  
  $item_data = $old_form['#data_list']->getItemData(
    $form_state['input']['old_type'],
    $form_state['input']['old_id']
  );

  $new_data = array('override-title' => $form_state['values']['override-title']);
  if ($item_data) {
    $new_data += $item_data;
  }

  $old_form['#data_list']->addItemData(
    $form_state['input']['old_type'],
    $form_state['input']['old_id'],
    $new_data
  );
  
  form_set_cache($form_build_id, $old_form, $old_form_state);
  _nt_feed_form_redirect($form_state);
}

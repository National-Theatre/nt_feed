<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

function nt_feed_ui_settings($form, &$form_status) {
  $form = array();
  $form = system_settings_form($form);
  return $form;
}

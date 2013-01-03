<?php
/**
 * @file
 * API description for hook for nt_feed module.
 *
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

function hook_nt_feed_register_settings_form_alter(&$form, &$form_state) {
  $flag = isset($form_state['item']->config) && is_array($form_state['item']->config);
  $form['MODULE'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('MODULE'),
    '#collapsible' => TRUE,
    'SomeConfig'  => array(
      '#type'          => 'textfield',
      '#title'         => t('SomeConfig'),
      '#default_value' => $flag? $form_state['item']->config['MODULE']['SomeConfig'] : 'default',
    ),
  );
}

function hook_nt_feed_register_settings_defaults_alter(&$settings) {
  $settings['MODULE'] = array(
    'Some' => 'Config',
  );
}

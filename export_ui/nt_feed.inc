<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

$plugin = array(
  'schema' => 'nt_feed',
  'menu' => array(
    'menu prefix' => 'admin/structure',
    'menu item' => 'inspire-me-feed',
    'menu title' => 'Inspire Me Feed',
    'menu description' => 'Associate menus, views, blocks, etc. with different contexts to structure your site.',
  ),
  'title singular' => t('Inspire Me'),
  'title singular proper' => t('Inspire Me Feed'),
  'title plural' => t('inspire feeds'),
  'title plural proper' => t('Inspire Me Feeds'),
  'form' => array(
    'settings' => 'nt_feed_form',
    'submit' => 'nt_feed_form_submit',
  ),
  'handler' => array(
    'class' => 'nt_feed_export_ui',
    'parent' => 'ctools_export_ui',
  ),
);

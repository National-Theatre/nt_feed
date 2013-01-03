<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */
?>
<div id="nt-feed-form-panel" class="clearfix">
  <h2><?php print t('Stream Feed'); ?></h2>
  <?php print drupal_render($form['plane']); ?>
  <?php print drupal_render($form['list']); ?>
</div>

<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */
?>
<table id="item-list-table">
  <thead>
    <tr>
      <th><?php print t('Weight'); ?></th>
      <th><?php print t('Data'); ?></th>
    </tr>
  </thead>
  <tbody>
    <?php print drupal_render_children($form); ?>
  </tbody>    
</table>

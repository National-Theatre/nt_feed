<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */
?>
<tr class="draggable">
  <td>
  </td>
  <td><?php print drupal_render($form['data']['weight']); ?></td>
  <td>
    <?php print drupal_render($form['data']['id']); ?>
    <?php print drupal_render($form['data']['feed_type']); ?>
    <?php print drupal_render($form['data']['type']); ?>
    <?php print drupal_render($form['data']['label']); ?>
  </td>
  <td>
    <?php print drupal_render($form['data']['del']); ?>
    <?php print drupal_render($form['data']['tools']); ?>
  </td>
</tr>

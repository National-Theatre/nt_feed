<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */
?>
<div id="<?php print $form['#css_id']; ?>" class="nt-feed-list-items-wrapper">
  <h3><?php print $form['#title']; ?></h3>
  <div class="nt-feed-list-item-wrapper">
    <?php foreach ($form['content'] as $item): ?>
      <?php if(is_array($item)) { print drupal_render($item); } ?>
    <?php endforeach; ?>
    <?php if ($form['more'] !== FALSE): ?>
    <?php print drupal_render($form['more']); ?>
    <?php endif; ?>
  </div>
</div>

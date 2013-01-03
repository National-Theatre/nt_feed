<?php
/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 *
 * Available variables:
 * - $form: The array containing the data for this item.
 *   - $form->#data->id: Id for this item.
 *   - $form->#data->html_id: Dom Id to use for this item.
 *   - $form->#data->title: Title string for this item.
 *   - $form->#data->date: Unix timestamp of creation date.
 *   - $form->#data->description: Description text for this item.
 *   - $form->#data->raw_data: Raw dump of this item from it's source.
 *   - $form->add: add button.
 *
 * @ingroup themeable
 */
?>
<div id="<?php print $form['#data']['html_id']; ?>" class="nt-feed-list-item" <?php if ($form['#data']['hidden']) { print 'style="display: none;"'; } ?>>
  <div class="handle">
    <?php print $form['#data']['title']; ?>
  </div>
  <?php print check_plain(strip_tags($form['#data']['description'])); ?>
  <?php print drupal_render($form['add']); ?>
</div>

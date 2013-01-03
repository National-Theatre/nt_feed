/**
 * @file
 * @copyright The Royal National Theatre
 * @author John-Paul Drawneek <jdrawneek@nationaltheatre.org.uk>
 */

(function ($) {
    Drupal.behaviors.nt_feed = {};
    Drupal.behaviors.nt_feed_ajax = {};
    Drupal.behaviors.nt_feed_ajax.attach = function(context) {
    }
    Drupal.behaviors.nt_feed.attach = function(context) {
        var $nt_feed_form = $('#nt-feed-plane > div');
        var $elements = $('#nt-feed-list-wrapper .nt-feed-list-item-wrapper .nt-feed-list-item');
        console.log($elements);
        $elements.draggable({
            distance: 4, // Pixels before dragging starts.
            handle: 'div.handle',
            revert: "invalid",
            helper: 'clone',
            appendTo: $nt_feed_form,
            opacity: 0.8,
            scope: 'nt-feed-form-panel',
            scroll: true,
            scrollSensitivity: 50,
            zIndex: 100,
            start: Drupal.nt_feed.startDrag,
            stop: Drupal.nt_feed.stopDrag
        });
        
        var $wrapper_elements = $('#nt-feed-list-wrapper .nt-feed-list-items-wrapper').not('.processed');
        $wrapper_elements.each(function() {
            var wrapper = $(this);
            var list = wrapper.find('.nt-feed-list-item-wrapper');
            list.hide();
            wrapper.delegate('h3', 'click', function(){
                list.toggle();
            });
            wrapper.addClass('processed');
        });
        $('#nt-feed-list-wrapper .nt-feed-list-items-wrapper:first .nt-feed-list-item-wrapper').toggle();
    }
    Drupal.nt_feed = {
        // Variable to prevent multiple requests.
        updatingElement: false,
        // Variables to allow delayed updates on textfields and textareas.
        updateDelayElement: false,
        updateDelay: false,
        // Variable holding the actively edited element (if any).
        activeElement: false,
        // Variable holding the active drag object (if any).
        activeDragUi: false,
        // Variables to keep trak of the current and previous drop target. Used to
        // prevent overlapping targets from being shown as active at the same time.
        activeDropzone: false,
        previousDropzones: [],
        // Variable of the time of the last update, used to prevent old data from
        // replacing newer updates.
        lastUpdateTime: 0,
        // Status of mouse click.
        mousePressed: 0,
        // Selector for a custom field configuration form.
        fieldConfigureForm: false
    };
    Drupal.nt_feed.startDrag = function (e, ui) {
        var $this = $(this);
        Drupal.nt_feed.activeDragUi = ui;
        $this.hide();
        Drupal.nt_feed.createDropTargets(this, ui.helper);
    }
    Drupal.nt_feed.stopDrag = function (e, ui) {
        var $this = $(this);
        $('#nt-feed-plane .nt-feed-placeholder').remove();
        $('#nt-feed-plane .nt-feed-placeholder-hover').removeClass('nt-feed-placeholder-hover');
    }
    Drupal.nt_feed.createDropTargets = function(draggable, helper) {
        var $placeholder = $('<div class="nt-feed-placeholder"></div>');
        var $elements = $('#nt-feed-plane .list-wrapper div.nt-feed-item:not(.nt-feed-empty-placeholder)').not('p').not(draggable).not(helper);
console.log($elements);
        if ($elements.length == 0) {
            // There are no form elements, insert a placeholder
            var $formBuilder = $('#nt-feed-plane .list-wrapper');
            $placeholder.height(100);
            $placeholder.appendTo($formBuilder);
        }
        else {
            $elements.each(function(i) {
              $placeholder.clone().insertAfter(this);
              // If the element is the first in its container, add a drop target above it.
              if (this == $(this).parent().children('.list-wrapper:not(.ui-draggable-dragging)').not(draggable)[0]) {
                $placeholder.clone().insertBefore(this);
              }
            });
        }

        // Enable the drop targets
        $('#nt-feed-plane').find('.nt-feed-placeholder, .nt-feed-empty-placeholder').droppable({
            greedy: true,
            scope: 'nt-feed-form-panel',
            tolerance: 'pointer',
            drop: Drupal.nt_feed.dropElement,
            over: Drupal.nt_feed.dropHover,
            out: Drupal.nt_feed.dropHover
        });
    };
    Drupal.nt_feed.dropHover = function (event, ui) {
        if (event.type == 'dropover') {
            // In the event that two droppables overlap, the latest one acts as the drop
            // target. If there is previous active droppable hide it temporarily.
            if (Drupal.nt_feed.activeDropzone) {
              $(Drupal.nt_feed.activeDropzone).css('display', 'none');
              Drupal.nt_feed.previousDropzones.push(Drupal.nt_feed.activeDropzone);
            }
            $(this).css({ height: ui.helper.height() + 'px', display: ''}).addClass('nt-feed-placeholder-hover');
            Drupal.nt_feed.activeDropzone = this;
        }
        else {
            $(this).css({ height: '', display: '' }).removeClass('nt-feed-placeholder-hover');

            // If this was active drop target, we remove the active state.
            if (Drupal.nt_feed.activeDropzone && Drupal.nt_feed.activeDropzone == this) {
              Drupal.nt_feed.activeDropzone = false;
            }
            // If there is a previous drop target that was hidden, restore it.
            if (Drupal.nt_feed.previousDropzones.length) {
              $(Drupal.nt_feed.previousDropzones).css('display', '');
              Drupal.nt_feed.activeDropzone = Drupal.nt_feed.previousDropzones.pop;
            }
        }
    };
    Drupal.nt_feed.dropElement = function (event, ui) {
        var $element = ui.draggable;
        var $placeholder = $(this);

        // If the element is a new field from the palette, update it with a real field.
        if ($element.is('.form-builder-palette-element')) {
            var name = 'new_' + new Date().getTime();
            // If this is a "unique" element, its element ID is hard-coded.
            if ($element.is('.form-builder-unique')) {
              name = $element.get(0).className.replace(/^.*?form-builder-element-([a-z0-9_]+).*?$/, '$1');
            }

            var $ajaxPlaceholder = $('<div class="form-builder-wrapper form-builder-new-field"><div id="form-builder-element-' + name + '" class="form-builder-element"><span class="progress">' + Drupal.t('Please wait...') + '</span></div></div>');

//            $.ajax({
//              url: $element.find('a').attr('href'),
//              type: 'GET',
//              dataType: 'json',
//              data: 'js=1&element_id=' + name,
//              success: Drupal.nt_feed.addElement
//            });

            $placeholder.replaceWith($ajaxPlaceholder);

            Drupal.nt_feed.updatingElement = true;
        }
        // Update the positions (weights and parents) in the form cache.
        else {
            $element.removeAttr('style');
            $placeholder.replaceWith($element);
            ui.helper.remove();
            //Drupal.formBuilder.updateElementPosition($element.get(0));
        }

        Drupal.nt_feed.activeDragUi = false;

        // Scroll the palette into view.
        //$(window).triggerHandler('scroll');
    };
})(jQuery);

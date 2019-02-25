<?php
/**
 * @file
 * This is the template file for the object page for web archives
 *
 * @TODO: add documentation about file and available variables
 */
?>

<div class="islandora-web-archive-object islandora">
  <div class="islandora-web-archive-content-wrapper clearfix">
    <?php if (isset($islandora_content)): ?>
      <div class="islandora-web-archive-content">
        <?php print $islandora_content; ?>
      </div>
    <?php endif; ?>
    <?php if (isset($islandora_entry_point_url)): ?>
    <p>Url: <b><?php print $archived_url ?></b></p>
    <p>Capture Date: <b><?php print $archived_date ?></b></p>
    <input type="button" class="form-submit" style="font-size: 18px" id="viewWR" value="Browse Web Archive"/>
    <script>
      jQuery("#viewWR").click(function() {
        jQuery.colorbox({'html': '<span class="wrHeader">Capture Date: <b><?php print $archived_date ?></b>&nbsp;&nbsp;Url: <b><?php print $archived_url ?></b></span><iframe src="<?php print $islandora_entry_point_url; ?>", seamless="seamless" style="min-width: 1000px; min-height: 800px;"></iframe>'});

        return false;
      });
    </script>
    <style>
      #cboxLoadedContent {margin-top: 28px}
      #cboxClose {top: 0px}
      .wrHeader {top: 0px; position: absolute; float: left}
    </style>
    <?php endif; ?>
    </div>
    <div class="islandora-web-archive-metadata">
      <?php print $description; ?>
    <div>
      <h2><?php print t('Download'); ?></h2>
        <ul>
          <?php if (isset($islandora_warc)): ?>
            <li>Warc: <?php print $islandora_warc; ?>
          <?php endif; ?>
          <?php if (isset($islandora_pdf)): ?>
            <li>PDF: <?php print $islandora_pdf; ?>
          <?php endif; ?>
          <?php if (isset($islandora_screenshot)): ?>
            <li>Screenshot: <?php print $islandora_screenshot; ?>
          <?php endif; ?>
          <?php if (isset($islandora_csv)): ?>
            <li>CSV: <?php print $islandora_csv; ?>
          <?php endif; ?>
        </ul>
    </div>
    <?php if ($parent_collections): ?>
      <div>
        <h2><?php print t('In collections'); ?></h2>
        <ul>
          <?php foreach ($parent_collections as $collection): ?>
            <li><?php print l($collection->label, "islandora/object/{$collection->id}"); ?></li>
          <?php endforeach; ?>
        </ul>
      </div>
    <?php endif; ?>
    <?php print $metadata; ?>
  </div>
</div>

<?php

/**
 * @file
 * Theme hooks.
 */

/**
 * Implements template_preprocess_HOOK().
 */
function template_preprocess_islandora_web_archive(array &$variables) {
  module_load_include('inc', 'islandora', 'includes/datastream');
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/solution_packs');
  module_load_include('inc', 'islandora', 'includes/authtokens');
  module_load_include('inc', 'islandora', 'includes/metadata');
  drupal_add_js('misc/form.js');
  drupal_add_js('misc/collapse.js');
  $object = $variables['object'];
  // We should eventually remove the DC object and dc_array code as it only
  // exists to not break legacy implementations.
  try {
    if (isset($object['DC']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['DC'])) {
      $dc = $object['DC']->content;
      $dc_object = DublinCore::importFromXMLString($dc);
    }
  }
  catch (Exception $e) {
    drupal_set_message(t('Error retrieving object %s %t', array('%s' => $object->id, '%t' => $e->getMessage())), 'error', FALSE);
  }
  $variables['islandora_dublin_core'] = isset($dc_object) ? $dc_object : NULL;
  $variables['dc_array'] = isset($dc_object) ? $dc_object->asArray() : array();
  $variables['islandora_object_label'] = $object->label;
  $variables['theme_hook_suggestions'][] = 'islandora_web_archive__' . str_replace(':', '_', $object->id);
  $variables['parent_collections'] = islandora_get_parents_from_rels_ext($object);
  $variables['metadata'] = islandora_retrieve_metadata_markup($object);
  $variables['description'] = islandora_retrieve_description_markup($object);

  // Original.
  if (isset($object['OBJ']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['OBJ'])) {
    $warc_url = url("islandora/object/{$object->id}/datastream/OBJ/view", array('absolute' => TRUE));
    $variables['islandora_warc'] = $warc_url;
  }

  // Thumbnail.
  if (isset($object['TN']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['TN'])) {
    $thumbnail_size_url = url("islandora/object/{$object->id}/datastream/TN/view");
    $params = array(
      'title' => $object->label,
      'path' => $thumbnail_size_url,
    );
    $variables['islandora_thumbnail_img'] = theme('image', $params);
  }

  // Download warc.
  if (isset($object['OBJ']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['OBJ'])) {
    $warc_url = url("islandora/object/{$object->id}/datastream/OBJ/{$object->label}.warc", array('absolute' => TRUE));
    $variables['islandora_warc'] = l($variables['islandora_object_label'] . '.warc', $warc_url, array('html' => TRUE));

    $res = init_wr_collection($warc_url, $object, $dc_object->asArray());
    if ($res) {
      $variables['archived_url'] = $res[0];
      $variables['archived_date'] = $res[1];
      $variables['islandora_entry_point_url'] = $res[2];
    }
  }

  // Download pdf.
  if (isset($object['PDF']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['PDF'])) {
    $pdf_url = url("islandora/object/{$object->id}/datastream/PDF/{$object->label}.pdf", array('absolute' => TRUE));
    $variables['islandora_pdf'] = l($variables['islandora_object_label'] . '.pdf', $pdf_url, array('html' => TRUE));
  }

  // Download screenshot.
  module_load_include('inc', 'islandora', 'includes/mimetype.utils');
  if (isset($object['SCREENSHOT']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['SCREENSHOT'])) {
    $file_ext = islandora_get_extension_for_mimetype($object['SCREENSHOT']->mimetype);
    $file_url = url("islandora/object/{$object->id}/datastream/SCREENSHOT/{$object->label}.{$file_ext}", array('absolute' => TRUE));
    $variables['islandora_screenshot'] = l($variables['islandora_object_label'] . '.' . $file_ext, $file_url, array('html' => TRUE));
  }

  // Download CSV.
  if (isset($object['WARC_CSV']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['WARC_CSV'])) {
    $csv_url = url("islandora/object/{$object->id}/datastream/WARC_CSV/{$object->label}.csv", array('absolute' => TRUE));
    $variables['islandora_csv'] = l($variables['islandora_object_label'] . '.csv', $csv_url, array('html' => TRUE));
  }

  // Medium size.
  if (isset($object['JPG']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['JPG'])) {
    $medium_size_url = url("islandora/object/{$object->id}/datastream/JPG/view");
    $params = array(
      'title' => $object->label,
      'path' => $medium_size_url,
    );
    $variables['islandora_medium_img'] = theme('image', $params);
    if (isset($medium_size_url)) {
      $variables['islandora_content'] = l($variables['islandora_medium_img'], $medium_size_url, array('html' => TRUE));
    }
    else {
      $variables['islandora_content'] = $variables['islandora_medium_img'];
    }
  }
}


function init_wr_collection($warc_url, $object, $dc) {
  if (init_wr_session($object)) {
    do_upload($object);
  }

  $coll_id = str_replace(':', '-', $object->id);

  //print_r(json_encode($dc));
  $url = trim(explode(',', $dc['dc:identifier']['value'], 3)[1]);
  $date = $dc['dc:date']['value'];
  $timestamp = str_replace('-', '', $date);

  $res = sprintf('http://isle.localdomain:8092/%s/%s/%smp_/%s', $_SESSION['WR_USER'],
                 $coll_id, $timestamp, $url);
//$dc_object['dc:identifier'][1]);

  //print_r($res);
  return [$url, $date, $res];
                 
}


function init_wr_session($object) {

  $orig_sesh = isset($_SESSION['WR_SESH']) ? $_SESSION['WR_SESH'] : null;
  $orig_user = isset($_SESSION['WR_USER']) ? $_SESSION['WR_USER'] : null;

  $data = query_wr_api('/auth/anon_user',
                       $post=true,
                       $json=null,
                       $get_cookie=true);

  $_SESSION['WR_USER'] = $data['user']['username'];

  $curr_sesh = $_SESSION['WR_SESH'];
  $curr_user = $_SESSION['WR_USER'];

  $is_new = ($curr_user != $orig_user) ||
            ($curr_sesh && $curr_sesh != $orig_sesh) ||
            !isset($_SESSION['WR_UPLOADED: '.$object->id]);

  $res = setcookie('__wr_sesh', $curr_sesh, 0, "/");

  //return [$curr_sesh, $curr_user, $is_new];
  return $is_new;
}

function do_upload($object) {
  $coll_id = str_replace(':', '-', $object->id);

  // Create collection
  $res = query_wr_api('/collections?user=' . $_SESSION['WR_USER'],
                      $post=true,
                      $json=array('title' => $coll_id,
                                  'external' => true));

  if (!isset($res['success']) && isset($res['error']) && $res['error'] != 'duplicate_name') {
    return;
  }

  // Download
  $file = tmpfile();

  try {
    $path = stream_get_meta_data($file)['uri'];
    $datastream = islandora_datastream_load('OBJ',  $object);
    $datastream->getContent($path);

    // Upload
    $res = query_wr_api(sprintf('/upload?force-coll=%s&filename=%s.warc.gz', $coll_id, $coll_id),
                        $post=false,
                        $json=null,
                        $get_cookie=false,
                        $put_file=$file);

    if (!isset($res['upload_id'])) {
      print_r($res['upload_id']);
      return;
    }

    while (true) {
      $res = query_wr_api(sprintf('/upload/%s?user=%s', $res['upload_id'], $_SESSION['WR_USER']));

      if (isset($res['done']) && $res['done']) {
        break;
      }

      usleep(0.5 * 1000000);
    }

    $_SESSION['WR_UPLOADED: ' . $object->id] = true;    

  } catch (Exception $e) {
    print_r($e);
  }

  fclose($file);
}

function query_wr_api($path, $post=false, $json=NULL, $get_cookie=false, $put_file=NULL) {
  $ch = curl_init();
  $headers = array();

  if (isset($_SESSION['WR_SESH'])) {
    $headers[] = 'Cookie: __wr_sesh=' . $_SESSION['WR_SESH'];
    //curl_setopt($ch, CURLOPT_COOKIE, '__wr_sesh=' . $_SESSION['WR_SESH']);
  }

  if ($json) {
    $headers[] = 'Content-Type: application/json';
  }

  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);

  if ($post) {
    curl_setopt($ch, CURLOPT_POST, true);
    if ($json) {
      curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($json));
    }
  } elseif ($put_file) {
    //$size = ftell($put_file);
    //$size = fileSize($put_filename);
    $size = filesize(stream_get_meta_data($put_file)['uri']);

    //print_r($size);

    fseek($put_file, 0);

    curl_setopt($ch, CURLOPT_PUT, true);
    //curl_setopt($ch, CURLOPT_UPLOAD, true);
    //curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_setopt($ch, CURLOPT_INFILE, $put_file);
    curl_setopt($ch, CURLOPT_INFILESIZE, $size);
    //curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "PUT");
    //curl_setopt($ch, CURLOPT_POSTFIELDS, $put);
  }

  if ($get_cookie) {
	  //curl_setopt($ch, CURLOPT_HEADERFUNCTION, 'parse_cookie_header');
    
    curl_setopt($ch, CURLOPT_HEADERFUNCTION, function($curl, $header) {
        $len = strlen($header);
        $match = 'set-cookie:';
        if (substr(strtolower($header), 0, strlen($match)) === $match) {
          $header = explode(':', $header, 2);
          $header = explode('=', trim(end($header)), 2);
          $header = explode(';', trim(end($header)), 2);
          $_SESSION['WR_SESH'] = $header[0];
        }
        return $len;
    });
  }

  $url = "http://nginx/api/v1" . $path;
  curl_setopt($ch, CURLOPT_URL,$url);
  $result=curl_exec($ch);
  curl_close($ch);
  return json_decode($result, $assoc=true);
}



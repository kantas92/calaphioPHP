<?php
/*
 * $RCSfile: G1MigrateMap.inc,v $
 *
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2005 Bharat Mediratta
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA  02110-1301, USA.
 */
/**
 * @package Migrate
 * @subpackage ClassInterfaces
 * @version $Revision: 1.5 $ $Date: 2005/08/23 03:49:46 $
 * @author Bharat Mediratta <bharat@menalto.com>
 */

/**
 * Interface class for the G1MigrateMap class.
 * (Class schema version: 1.0)
 *
 * This file is automatically generated from XML embedded in the
 * class, combined with the ClassCode.xsl file.  You should not modify
 * it by hand, as your changes will be lost next time this file is
 * rebuilt.  If you want to add more getters/setters, you should add
 * them in the core class.  If you want to change the format of this
 * file, then you should edit the XSL template.
 *
 * @package Migrate
 * @subpackage Classes
 */
class G1MigrateMap extends G1MigrateMap_core {
  
    /**
     * Return the name of this class with the proper mix of uppercase and
     * lowercase (get_class() always returns lowercase)
     *
     * @return string class name
     */
    function getClassName() {
	return 'G1MigrateMap';
    }

    /** 
     * Return the path to the PHP file for this class, relative to the gallery2 dir
     * 
     * @return string path
     */ 
    function getClassFile() { 
        return 'modules/' .  
            basename(dirname(dirname(dirname(__FILE__)))) .  
            '/classes/G1MigrateMap.class'; 
    }
    
    /**
     * Get meta information about this class' map 
     *
     * @return array map member => type
     */
    function getMapInfo() {
	$info = array();
	$info['members']['itemId'] = STORAGE_TYPE_INTEGER;
	$info['members']['g1album'] = STORAGE_TYPE_STRING;
	$info['members']['g1item'] = STORAGE_TYPE_STRING;
	return $info; 
    }

    /**
     * Add an entry to this map
     *
     * @param array an associative array of the entry data
     * @return object GalleryStatus a status code
     * @static
     */
    function addMapEntry($data) {
	global $gallery;
        foreach (array('itemId', 'g1album', 'g1item', ) as $key) {
	    if (!array_key_exists($key, $data)) {
	        return GalleryStatus::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__,
                                            'Missing parameter: ' . $key);
	    }
        }

	$storage =& $gallery->getStorage();
	$ret = $storage->addMapEntry('G1MigrateMap', $data);
	if ($ret->isError()) {
	    return $ret->wrap(__FILE__, __LINE__);
	}
	return GalleryStatus::success();
    }

    /**
     * Remove an entry from this map
     *
     * @param array an associative array of the entry data to match and remove
     * @return object GalleryStatus a status code
     * @static
     */
    function removeMapEntry($data) {
	global $gallery;

	if (sizeof($data) == 0) {
	    return GalleryStatus::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__);
	}

	$storage =& $gallery->getStorage();
	$ret = $storage->removeMapEntry('G1MigrateMap', $data);
	if ($ret->isError()) {
	    return $ret->wrap(__FILE__, __LINE__);
	}
	return GalleryStatus::success();
    }

    /**
     * Remove ALL entries from this map.. use with caution!
     *
     * @return object GalleryStatus a status code
     * @static
     */
    function removeAllMapEntries() {
	global $gallery;

	$storage =& $gallery->getStorage();
	$ret = $storage->removeAllMapEntries('G1MigrateMap');
	if ($ret->isError()) {
	    return $ret->wrap(__FILE__, __LINE__);
	}
	return GalleryStatus::success();
    }

    /**
     * Update an entry in this map
     *
     * @param array the entry to match
     * @param array the values to change
     * @return object GalleryStatus a status code
     * @static
     */
    function updateMapEntry($match, $change) {
	global $gallery;

	if (sizeof($match) == 0 || sizeof($change) == 0) {
	    return GalleryStatus::error(ERROR_BAD_PARAMETER, __FILE__, __LINE__);
	}

	$storage =& $gallery->getStorage();
	$ret = $storage->updateMapEntry('G1MigrateMap', $match, $change);
	if ($ret->isError()) {
	    return $ret->wrap(__FILE__, __LINE__);
	}
	return GalleryStatus::success();
    }
  
}
?>

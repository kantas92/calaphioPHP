<?php
/*
 * $RCSfile: theme.inc,v $
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
 * @version $Revision: 1.7 $ $Date: 2005/09/10 20:28:16 $
 * @package Gallery
 * @author Alan Harder <alan.harder@sun.com>
 */

/**
 * This implements gallery theme combining thumb-view, image-view
 * and slideshow into a single dynamic page.
 *
 * @package GalleryTheme
 * @subpackage Theme
 */
class HybridTheme extends GalleryTheme {

    /**
     * Constructor
     */
    function HybridTheme() {
	global $gallery;

	$this->setId('hybrid');
	$this->setName($gallery->i18n('Hybrid'));
	$this->setDescription(
	    $gallery->i18n('Combined album/image/slideshow dynamic view'));
	$this->setVersion('1.0.0');
	$this->setRequiredCoreApi(array(6, 0));
	$this->setRequiredThemeApi(array(2, 0));
	$this->setStandardSettings(array(
	    'rows' => 30, 'columns' => 2,
	    'albumFrame' => '', 'itemFrame' => '', 'colorpack' => '',
	    'sidebarBlocks' => serialize(array(
				array('search.SearchBlock', array('showAdvancedLink' => true)),
				array('core.ItemLinks', array('useDropdown' => false)),
				array('core.PeerList', array()),
				array('imageblock.ImageBlock', array()))),
	    'albumBlocks' => serialize(array(
				array('comment.ViewComments', array()))),
	    'photoBlocks' => serialize(array(
				array('exif.ExifInfo', array()),
				array('comment.ViewComments', array()))),
	));
    }

    /**
     * @see GalleryTheme::showAlbumPage
     */
    function showAlbumPage(&$template, $item, $params, $childIds) {
	$ret = $this->loadCommonTemplateData($template, $item, $params,
	    array('owner', 'viewCount', 'parents', 'systemLinks', 'itemLinks',
		  'childItemLinksDetailed', 'permissions', 'jumpRange'),
	    $childIds);
	if ($ret->isError()) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}
	$theme =& $template->getVariableByReference('theme');

	list ($ret, $imageWidths, $imageHeights) =
	    $this->_buildItemList($childIds, $theme['children']);
	if ($ret->isError()) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}

	$theme['imageWidths'] = implode(',', $imageWidths);
	$theme['imageHeights'] = implode(',', $imageHeights);
	$theme['imageCount'] = count($imageWidths);
	$theme['columnWidthPct'] = floor(90 / $params['columns']);

	$showId = GalleryUtilities::getRequestVariables('showId');
	if (!empty($showId)) {
	    /* Start with initial view of this item */
	    foreach ($theme['children'] as $tmp) {
		if ($tmp['id'] == $showId) {
		    if (isset($tmp['imageIndex'])) {
			$theme['viewIndex'] = $tmp['imageIndex'];
			/* Set cookie to popup image just once */
			setcookie('G2_hybrid_view', '1');
		    }
		    break;
		}
	    }
	}

	$template->head('themes/hybrid/templates/header.tpl');
	return array(GalleryStatus::success(), 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showPhotoPage
     */
    function showPhotoPage(&$template, $item, $params) {
	list ($detail, $renderId) = GalleryUtilities::getRequestVariables('detail', 'renderId');
	if (!empty($renderId)) {
	    /* Render this item */
	    list ($ret, $image) = GalleryCoreApi::loadEntitiesById($renderId);
	    if ($ret->isError()) {
		return array($ret->wrap(__FILE__, __LINE__), null);
	    }
	    $template->setVariable('theme', array('item' => $item->getMemberData(),
						  'image' => $image->getMemberData()));
	    return array(GalleryStatus::success(), 'render.tpl');
	}

	if (!empty($detail)) {
	    /* View item details */
	    $ret = $this->loadCommonTemplateData($template, $item, $params,
						 array('owner', 'viewCount'));
	    if ($ret->isError()) {
		return array($ret->wrap(__FILE__, __LINE__), null);
	    }
	    $theme =& $template->getVariableByReference('theme');
	    list ($ret, $thumbnail) =
		GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getId()));
	    if ($ret->isError()) {
		return array($ret->wrap(__FILE__, __LINE__), null);
	    }
	    if (!empty($thumbnail)) {
		$theme['thumbnail'] = $thumbnail[$item->getId()]->getMemberData();
	    }
	    $template->style('themes/hybrid/theme.css');
	    return array(GalleryStatus::success(), 'detail.tpl');
	}

	/* Otherwise, redirect to parent with initial view of this item  */
	list ($ret, $parent) = GalleryCoreApi::loadEntitiesById($item->getParentId());
	if ($ret->isError()) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}
	$theme =& $template->getVariableByReference('theme');
	list ($ret, $childIds) =
	    GalleryCoreApi::fetchChildItemIds($parent, null, null, $theme['actingUserId']);
	if ($ret->isError()) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}
	$perPage = $this->getPageSize($params);
	$page = 1;
	for ($i = 0; $i < count($childIds); $i++) {
	    if ($childIds[$i] == $item->getId()) {
		$page = ceil(($i + 1) / $perPage);
		break;
	    }
	}
	$redirect = array('view' => 'core.ShowItem', 'itemId' => $parent->getId());
	if ($page != 1) {
	    $redirect['page'] = $page;
	}
	$redirect['showId'] = $item->getId();
	return array(GalleryStatus::success(), array('redirect' => $redirect));
    }

    /**
     * @see GalleryTheme::showModulePage
     */
    function showModulePage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params, array('parents', 'systemLinks'));
	if ($ret->isError()) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}

	return array(GalleryStatus::success(), 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showAdminPage
     */
    function showAdminPage(&$template, $item, $params, $templateFile) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params, array('parents', 'systemLinks'));
	if ($ret->isError()) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}

	return array(GalleryStatus::success(), 'theme.tpl');
    }

    /**
     * @see GalleryTheme::showErrorPage
     */
    function showErrorPage(&$template) {
	return array(GalleryStatus::success(), 'error.tpl');
    }

    /**
     * @see GalleryTheme::showProgressBarPage
     */
    function showProgressBarPage(&$template, $item, $params) {
	$ret = $this->loadCommonTemplateData(
	    $template, $item, $params, array('parents', 'systemLinks'));
	if ($ret->isError()) {
	    return array($ret->wrap(__FILE__, __LINE__), null);
	}

	return array(GalleryStatus::success(), 'theme.tpl');
    }

    /**
     * Prepare data about items in this album.
     * @access private
     */
    function _buildItemList($childIds, &$children) {
	$imageWidths = $imageHeights = $childItems = array();
	if (!empty($childIds)) {
	    $ret = GalleryCoreApi::studyPermissions($childIds);
	    if ($ret->isError()) {
		return array($ret->wrap(__FILE__, __LINE__), null, null);
	    }
	    list ($ret, $childItems) = GalleryCoreApi::loadEntitiesById($childIds);
	    if ($ret->isError()) {
		return array($ret->wrap(__FILE__, __LINE__), null, null);
	    }
	    list ($ret, $preferredFullImages) =
		GalleryCoreApi::fetchPreferredsByItemIds($childIds);
	    if ($ret->isError()) {
		return array($ret->wrap(__FILE__, __LINE__), null, null);
	    }
	    list ($ret, $resizedImages) =
		GalleryCoreApi::fetchResizesByItemIds($childIds);
	    if ($ret->isError()) {
		return array($ret->wrap(__FILE__, __LINE__), null, null);
	    }
	    list ($ret, $thumbnails) =
		GalleryCoreApi::fetchThumbnailsByItemIds($childIds);
	    if ($ret->isError()) {
		return array($ret->wrap(__FILE__, __LINE__), null, null);
	    }
	}
	$i = -1;
	foreach ($childItems as $child) {
	    $i++;
	    $childId = $child->getId();
	    if (isset($thumbnails[$childId])) {
		if (!($thumbnails[$childId]->getWidth() && $thumbnails[$childId]->getHeight())) {
		    list ($ret, $thumbnails[$childId]) =
			GalleryCoreApi::rebuildDerivativeCache($thumbnails[$childId]->getId());
		    if ($ret->isError()) {
			return array($ret->wrap(__FILE__, __LINE__), null, null);
		    }
		}
		$children[$i]['thumbnail'] = $thumbnails[$childId]->getMemberData();
	    }

	    if (!GalleryUtilities::isA($child, 'GalleryDataItem')) {
		continue;
	    }

	    list ($ret, $permissions) = GalleryCoreApi::getPermissions($childId);
	    if ($ret->isError()) {
		return array($ret->wrap(__FILE__, __LINE__), null, null);
	    }

	    $images = array();
	    if (isset($permissions['core.viewSource'])) {
		/* Full size; check for preferred copy */
		if (isset($preferredFullImages[$childId])) {
		    $images[] = $preferredFullImages[$childId];
		} else if ($child->canBeViewedInline()) {
		    $images[] = $child;
		}
	    }
	    if (isset($permissions['core.viewResizes']) && isset($resizedImages[$childId])) {
		foreach ($resizedImages[$childId] as $resize) {
		    $images[] = $resize;
		}
	    }
	    if (!isset($permissions['core.viewSource']) && isset($thumbnails[$childId])) {
		/* Only use thumbnail as view if we can't link to original file */
		$images[] = $thumbnails[$childId];
	    }

	    if (!empty($images)) {
		$image = $images[0];

		/* Rebuild derivative if needed so width/height known.. */
		if (GalleryUtilities::isA($image, 'GalleryDerivativeImage')
			&& !($image->getWidth() && $image->getHeight())) {
		    list ($ret, $image) = GalleryCoreApi::rebuildDerivativeCache($image->getId());
		    if ($ret->isError()) {
			return array($ret->wrap(__FILE__, __LINE__), null, null);
		    }
		}
		$image = $image->getMemberData();
		$children[$i]['image'] = $image;
		$children[$i]['imageIndex'] = count($imageWidths);
		if (GalleryUtilities::isExactlyA($child, 'GalleryPhotoItem') &&
			$image['width'] > 0 && $image['height'] > 0) {
		    /* Display in <img> */
		    $imageWidths[] = $image['width'];
		    $imageHeights[] = $image['height'];
		} else {
		    /* Item must render itself */
		    $children[$i]['renderItem'] = 1;
		    $imageWidths[] = $imageHeights[] = -1;
		}
	    }
	}
	return array(GalleryStatus::success(), $imageWidths, $imageHeights);
    }
}
?>

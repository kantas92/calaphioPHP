<?php
/*
 * $RCSfile: module.inc,v $
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
 * @version $Revision: 1.25 $ $Date: 2005/09/10 20:28:15 $
 * @package uploadapplet
 * @author Pierre-Luc Paour <paour@users.sourceforge.net>
 */

class UploadAppletModule extends GalleryModule {

    function UploadAppletModule() {
	global $gallery;
	$this->setId('uploadapplet');
	$this->setName($gallery->i18n('Upload Applet'));
	$this->setDescription($gallery->i18n('Easy to use upload applet for media'));
	$this->setVersion('1.0.0');
	$this->setGroup('import', $this->translate('Import'));
	$this->setCallbacks('');
	$this->setRequiredCoreApi(array(6, 0));
	$this->setRequiredModuleApi(array(2, 0));
    }

    /**
     * @see GalleryModule::performFactoryRegistrations()
     */
    function performFactoryRegistrations() {
	$ret = GalleryCoreApi::registerFactoryImplementation(
	    'ItemAddPlugin', 'ItemAddUploadApplet', 'ItemAddUploadApplet',
	    'modules/uploadapplet/ItemAddUploadApplet.inc', 'uploadapplet', null);
	if ($ret->isError()) {
	    return $ret->wrap(__FILE__, __LINE__);
	}

	return GalleryStatus::success();
    }

    /**
     * @see GalleryModule::isRecommendedDuringInstall()
     */
    function isRecommendedDuringInstall() {
	return false;
    }

    /**
     * @see GalleryModule::autoConfigure()
     */
    function autoConfigure() {
	/* We don't require any special configuration */
	return array(GalleryStatus::success(), true);
    }
}
?>

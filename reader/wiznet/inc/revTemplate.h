/*
 * rev.h
 *
 *  Created on: 25 Oct 2009
 *      Author: Wouter
 */

#ifndef REV_H_
#define REV_H_

const char csWCREV[] = "Revision $WCREV$\0";  // Replaced with the highest commit revision in the working copy.
const char csWCDATE[] = "$WCDATE$\0";  // Replaced with the commit date/time of the highest commit revision.
const char csWCURL[] = "$WCURL$\0"; // Replaced with the repository URL of the working copy path passed to SubWCRev.

#endif /* REV_H_ */

/*
 * e-source-mdn.h
 *
 * This library is free software you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#if !defined (__LIBEDATASERVER_H_INSIDE__) && !defined (LIBEDATASERVER_COMPILATION)
#error "Only <libedataserver/libedataserver.h> should be included directly."
#endif

#ifndef E_SOURCE_MDN_H
#define E_SOURCE_MDN_H

#include <libedataserver/e-source-enums.h>
#include <libedataserver/e-source-extension.h>

/* Standard GObject macros */
#define E_TYPE_SOURCE_MDN \
	(e_source_mdn_get_type ())
#define E_SOURCE_MDN(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST \
	((obj), E_TYPE_SOURCE_MDN, ESourceMDN))
#define E_SOURCE_MDN_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_CAST \
	((cls), E_TYPE_SOURCE_MDN, ESourceMDNClass))
#define E_IS_SOURCE_MDN(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE \
	((obj), E_TYPE_SOURCE_MDN))
#define E_IS_SOURCE_MDN_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_TYPE \
	((cls), E_TYPE_SOURCE_MDN))
#define E_SOURCE_MDN_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS \
	((obj), E_TYPE_SOURCE_MDN, ESourceMDNClass))

/**
 * E_SOURCE_EXTENSION_MDN:
 *
 * Pass this extension name to e_source_get_extension() to access
 * #ESourceMDN.  This is also used as a group name in key files.
 *
 * Since: 3.6
 **/
#define E_SOURCE_EXTENSION_MDN "Message Disposition Notifications"

G_BEGIN_DECLS

typedef struct _ESourceMDN ESourceMDN;
typedef struct _ESourceMDNClass ESourceMDNClass;
typedef struct _ESourceMDNPrivate ESourceMDNPrivate;

/**
 * ESourceMDN:
 *
 * Contains only private data that should be read and manipulated using the
 * functions below.
 *
 * Since: 3.6
 **/
struct _ESourceMDN {
	ESourceExtension parent;
	ESourceMDNPrivate *priv;
};

struct _ESourceMDNClass {
	ESourceExtensionClass parent_class;
};

GType		e_source_mdn_get_type
					(void) G_GNUC_CONST;
EMdnResponsePolicy
		e_source_mdn_get_response_policy
					(ESourceMDN *extension);
void		e_source_mdn_set_response_policy
					(ESourceMDN *extension,
					 EMdnResponsePolicy response_policy);

G_END_DECLS

#endif /* E_SOURCE_MDN_H */

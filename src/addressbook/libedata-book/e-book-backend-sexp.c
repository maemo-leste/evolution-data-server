/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 1999-2008 Novell, Inc. (www.novell.com)
 * Copyright (C) 2012 Intel Corporation
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Chris Lahey <clahey@ximian.com>
 *          Tristan Van Berkom <tristanvb@openismus.com>
 */

/**
 * SECTION: e-book-backend-sexp
 * @include: libedata-book/libedata-book.h
 * @short_description: A utility for comparing #EContact objects or vcards with search expressions.
 *
 * This API is an all purpose utility for comparing #EContact objects with search expressions generated by #EBookQuery.
 */
#include "e-book-backend-sexp.h"

#include <glib/gi18n.h>
#include <locale.h>
#include <string.h>

#define E_BOOK_BACKEND_SEXP_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE \
	((obj), E_TYPE_BOOK_BACKEND_SEXP, EBookBackendSExpPrivate))

G_DEFINE_TYPE (EBookBackendSExp, e_book_backend_sexp, G_TYPE_OBJECT)

typedef struct _SearchContext SearchContext;
typedef gboolean (*CompareFunc) (const gchar *, const gchar *, const gchar *);

struct _EBookBackendSExpPrivate {
	ESExp *search_sexp;
	gchar *text;
	SearchContext *search_context;
};

struct _SearchContext {
	EContact *contact;
};

static gboolean
compare_im (EContact *contact,
            const gchar *str,
            const gchar *region,
            CompareFunc compare,
            EContactField im_field)
{
	GList    *aims, *l;
	gboolean  found_it = FALSE;

	aims = e_contact_get (contact, im_field);

	for (l = aims; l != NULL; l = l->next) {
		gchar *im = (gchar *) l->data;

		if (im && compare (im, str, region)) {
			found_it = TRUE;
			break;
		}
	}

	e_contact_attr_list_free (aims);

	return found_it;
}

static gboolean
compare_im_aim (EContact *contact,
                const gchar *str,
                const gchar *region,
                CompareFunc compare)
{
	return compare_im (contact, str, region, compare, E_CONTACT_IM_AIM);
}

static gboolean
compare_im_msn (EContact *contact,
                const gchar *str,
                const gchar *region,
                CompareFunc compare)
{
	return compare_im (contact, str, region, compare, E_CONTACT_IM_MSN);
}

static gboolean
compare_im_skype (EContact *contact,
                  const gchar *str,
                  const gchar *region,
                  CompareFunc compare)
{
	return compare_im (contact, str, region, compare, E_CONTACT_IM_SKYPE);
}

static gboolean
compare_im_google_talk (EContact *contact,
                        const gchar *str,
                        const gchar *region,
                        CompareFunc compare)
{
	return compare_im (contact, str, region, compare, E_CONTACT_IM_GOOGLE_TALK);
}

static gboolean
compare_im_icq (EContact *contact,
                const gchar *str,
                const gchar *region,
                CompareFunc compare)
{
	return compare_im (contact, str, region, compare, E_CONTACT_IM_ICQ);
}

static gboolean
compare_im_yahoo (EContact *contact,
                  const gchar *str,
                  const gchar *region,
                  CompareFunc compare)
{
	return compare_im (contact, str, region, compare, E_CONTACT_IM_YAHOO);
}

static gboolean
compare_im_gadugadu (EContact *contact,
                     const gchar *str,
                     const gchar *region,
                     CompareFunc compare)
{
	return compare_im (contact, str, region, compare, E_CONTACT_IM_GADUGADU);
}

static gboolean
compare_im_jabber (EContact *contact,
                   const gchar *str,
                   const gchar *region,
                   CompareFunc compare)
{
	return compare_im (contact, str, region, compare, E_CONTACT_IM_JABBER);
}

static gboolean
compare_im_groupwise (EContact *contact,
                      const gchar *str,
                      const gchar *region,
                      CompareFunc compare)
{
	return compare_im (contact, str, region, compare, E_CONTACT_IM_GROUPWISE);
}

static gboolean
compare_email (EContact *contact,
               const gchar *str,
               const gchar *region,
               CompareFunc compare)
{
	gboolean rv = FALSE;
	GList *list, *l;

	list = e_contact_get (contact, E_CONTACT_EMAIL);

	for (l = list; l; l = l->next) {
		const gchar *email = l->data;

		rv = email && compare (email, str, region);

		if (rv)
			break;
	}

	e_contact_attr_list_free (list);

	return rv;
}

static gboolean
compare_phone (EContact *contact,
               const gchar *str,
               const gchar *region,
               CompareFunc compare)
{
	GList *list, *l;
	gboolean rv = FALSE;

	list = e_contact_get (contact, E_CONTACT_TEL);

	for (l = list; l; l = l->next) {
		const gchar *phone = l->data;

		rv = phone && compare (phone, str, region);

		if (rv)
			break;
	}

	e_contact_attr_list_free (list);

	return rv;
}

static gboolean
compare_name (EContact *contact,
              const gchar *str,
              const gchar *region,
              CompareFunc compare)
{
	const gchar *name;

	name = e_contact_get_const (contact, E_CONTACT_FULL_NAME);
	if (name && compare (name, str, region))
		return TRUE;

	name = e_contact_get_const (contact, E_CONTACT_FAMILY_NAME);
	if (name && compare (name, str, region))
		return TRUE;

	name = e_contact_get_const (contact, E_CONTACT_GIVEN_NAME);
	if (name && compare (name, str, region))
		return TRUE;

	name = e_contact_get_const (contact, E_CONTACT_NICKNAME);
	if (name && compare (name, str, region))
		return TRUE;

	return FALSE;
}

static gboolean
compare_photo_uri (EContact *contact,
                   const gchar *str,
                   const gchar *region,
                   CompareFunc compare)
{
	EContactPhoto *photo;
	gboolean ret_val = FALSE;

	photo = e_contact_get (contact, E_CONTACT_PHOTO);

	if (photo) {
		/* Compare the photo uri with the string */
		if ((photo->type == E_CONTACT_PHOTO_TYPE_URI)
		     && compare (photo->data.uri, str, region)) {
			ret_val = TRUE;
		}
		e_contact_photo_free (photo);
	}
	return ret_val;
}

static gboolean
compare_address (EContact *contact,
                 const gchar *str,
                 const gchar *region,
                 CompareFunc compare)
{

	gint i;
	gboolean rv = FALSE;

	for (i = E_CONTACT_FIRST_ADDRESS_ID; i <= E_CONTACT_LAST_ADDRESS_ID; i++) {
		EContactAddress *address = e_contact_get (contact, i);
		if (address) {
			rv = (address->po && compare (address->po, str, region)) ||
				(address->street && compare (address->street, str, region)) ||
				(address->ext && compare (address->ext, str, region)) ||
				(address->locality && compare (address->locality, str, region)) ||
				(address->region && compare (address->region, str, region)) ||
				(address->code && compare (address->code, str, region)) ||
				(address->country && compare (address->country, str, region));

			e_contact_address_free (address);

			if (rv)
				break;
		}
	}

	return rv;

}

static gboolean
compare_category (EContact *contact,
                  const gchar *str,
                  const gchar *region,
                  CompareFunc compare)
{
	GList *categories;
	GList *iterator;
	gboolean ret_val = FALSE;

	categories = e_contact_get (contact, E_CONTACT_CATEGORY_LIST);

	for (iterator = categories; iterator; iterator = iterator->next) {
		const gchar *category = iterator->data;

		if (compare (category, str, region)) {
			ret_val = TRUE;
			break;
		}
	}

	g_list_foreach (categories, (GFunc) g_free, NULL);
	g_list_free (categories);

	return ret_val;
}

static gboolean
compare_date (EContactDate *date,
              const gchar *str,
              const gchar *region,
              CompareFunc compare)
{
	gchar *date_str = e_contact_date_to_string (date);
	gboolean ret_val = FALSE;

	if (date_str) {
		if (compare (date_str, str, region)) {
			ret_val = TRUE;
		}
		g_free (date_str);
	}
	return ret_val;
}

enum prop_type {
	PROP_TYPE_NORMAL,
	PROP_TYPE_LIST,
	PROP_TYPE_DATE
};

static struct prop_info {
	EContactField field_id;
	const gchar *query_prop;
	enum prop_type prop_type;
	gboolean (*list_compare) (EContact *contact,
				  const gchar *str,
				  const gchar *region,
				  CompareFunc compare);

} prop_info_table[] = {
#define NORMAL_PROP(f,q) {f, q, PROP_TYPE_NORMAL, NULL}
#define LIST_PROP(q,c) {0, q, PROP_TYPE_LIST, c}
#define DATE_PROP(f,q) {f, q, PROP_TYPE_DATE, NULL}

	/* query prop,   type,              list compare function */
	NORMAL_PROP ( E_CONTACT_FILE_AS, "file_as" ),
	NORMAL_PROP ( E_CONTACT_UID, "id" ),
	LIST_PROP ( "full_name", compare_name), /* not really a list, but we need to compare both full and surname */
	LIST_PROP ( "photo", compare_photo_uri ), /* not really a list, but we need to compare the uri in the struct */
	DATE_PROP ( E_CONTACT_BIRTH_DATE, "birth_date" ),
	DATE_PROP ( E_CONTACT_ANNIVERSARY, "anniversary" ),
	NORMAL_PROP ( E_CONTACT_GIVEN_NAME, "given_name"),
	NORMAL_PROP ( E_CONTACT_FAMILY_NAME, "family_name"),
	NORMAL_PROP ( E_CONTACT_HOMEPAGE_URL, "url"),
	NORMAL_PROP ( E_CONTACT_BLOG_URL, "blog_url"),
	NORMAL_PROP ( E_CONTACT_CALENDAR_URI, "calurl"),
	NORMAL_PROP ( E_CONTACT_FREEBUSY_URL, "fburl"),
	NORMAL_PROP ( E_CONTACT_ICS_CALENDAR, "icscalendar"),
	NORMAL_PROP ( E_CONTACT_VIDEO_URL, "video_url"),

	NORMAL_PROP ( E_CONTACT_MAILER, "mailer"),
	NORMAL_PROP ( E_CONTACT_ORG, "org"),
	NORMAL_PROP ( E_CONTACT_ORG_UNIT, "org_unit"),
	NORMAL_PROP ( E_CONTACT_OFFICE, "office"),
	NORMAL_PROP ( E_CONTACT_TITLE, "title"),
	NORMAL_PROP ( E_CONTACT_ROLE, "role"),
	NORMAL_PROP ( E_CONTACT_MANAGER, "manager"),
	NORMAL_PROP ( E_CONTACT_ASSISTANT, "assistant"),
	NORMAL_PROP ( E_CONTACT_NICKNAME, "nickname"),
	NORMAL_PROP ( E_CONTACT_SPOUSE, "spouse" ),
	NORMAL_PROP ( E_CONTACT_NOTE, "note"),
	LIST_PROP ( "im_aim",    compare_im_aim ),
	LIST_PROP ( "im_msn",    compare_im_msn ),
	LIST_PROP ( "im_skype",    compare_im_skype ),
	LIST_PROP ( "im_google_talk",    compare_im_google_talk ),
	LIST_PROP ( "im_icq",    compare_im_icq ),
	LIST_PROP ( "im_jabber", compare_im_jabber ),
	LIST_PROP ( "im_yahoo",  compare_im_yahoo ),
	LIST_PROP ( "im_gadugadu",  compare_im_gadugadu ),
	LIST_PROP ( "im_groupwise", compare_im_groupwise ),
	LIST_PROP ( "email",     compare_email ),
	LIST_PROP ( "phone",     compare_phone ),
	LIST_PROP ( "address",   compare_address ),
	LIST_PROP ( "category_list",  compare_category ),
};

static ESExpResult *
entry_compare (SearchContext *ctx,
               struct _ESExp *f,
               gint argc,
               struct _ESExpResult **argv,
               CompareFunc compare)
{
	ESExpResult *r;
	gint truth = FALSE;

	if ((argc == 2
		&& argv[0]->type == ESEXP_RES_STRING
		&& argv[1]->type == ESEXP_RES_STRING) ||
	    (argc == 3
		&& argv[0]->type == ESEXP_RES_STRING
		&& argv[1]->type == ESEXP_RES_STRING
		&& argv[2]->type == ESEXP_RES_STRING)) {
		gchar *propname;
		struct prop_info *info = NULL;
		const gchar *region = NULL;
		gint i;
		gboolean any_field;
		gboolean saw_any = FALSE;

		if (argc > 2)
			region = argv[2]->value.string;

		propname = argv[0]->value.string;

		any_field = !strcmp (propname, "x-evolution-any-field");
		for (i = 0; i < G_N_ELEMENTS (prop_info_table); i++) {
			if (any_field
			    || !strcmp (prop_info_table[i].query_prop, propname)) {
				saw_any = TRUE;
				info = &prop_info_table[i];

				if (any_field && info->field_id == E_CONTACT_UID) {
					/* We need to skip UID from any field contains search
					 * any-field search should be supported for the
					 * visible fields only.
					 */
					truth = FALSE;
				}
				else if (info->prop_type == PROP_TYPE_NORMAL) {
					const gchar *prop = NULL;
					/* straight string property matches */

					prop = e_contact_get_const (ctx->contact, info->field_id);

					if (prop && compare (prop, argv[1]->value.string, region)) {
						truth = TRUE;
					}
					if ((!prop) && compare ("", argv[1]->value.string, region)) {
						truth = TRUE;
					}
				}
				else if (info->prop_type == PROP_TYPE_LIST) {
					/* the special searches that match any of the list elements */
					truth = info->list_compare (ctx->contact, argv[1]->value.string, region, compare);
				}
				else if (info->prop_type == PROP_TYPE_DATE) {
					/* the special searches that match dates */
					EContactDate *date;

					date = e_contact_get (ctx->contact, info->field_id);

					if (date) {
						truth = compare_date (date, argv[1]->value.string, region, compare);
						e_contact_date_free (date);
					}
				} else {
					g_warn_if_reached ();

					saw_any = FALSE;
					break;
				}

				/* if we're looking at all fields and find a match,
				 * or if we're just looking at this one field,
				 * break. */
				if ((any_field && truth)
				    || !any_field)
					break;
			}
		}

		if (!saw_any) {
			/* propname didn't match to any of our known "special" properties,
			 * so try to find if it isn't a real field and if so, then compare
			 * against value in this field only */
			EContactField fid = e_contact_field_id (propname);

			if (fid >= E_CONTACT_FIELD_FIRST && fid < E_CONTACT_FIELD_LAST) {
				const gchar *prop = e_contact_get_const (ctx->contact, fid);

				if (prop && compare (prop, argv[1]->value.string, region)) {
					truth = TRUE;
				}

				if ((!prop) && compare ("", argv[1]->value.string, region)) {
					truth = TRUE;
				}
			} else {
				/* it is not direct EContact known field, so try to find
				 * it in EVCard attributes */
				GList *a, *attrs = e_vcard_get_attributes (E_VCARD (ctx->contact));
				for (a = attrs; a && !truth; a = a->next) {
					EVCardAttribute *attr = (EVCardAttribute *) a->data;
					if (g_ascii_strcasecmp (e_vcard_attribute_get_name (attr), propname) == 0) {
						GList *l, *values = e_vcard_attribute_get_values (attr);

						for (l = values; l && !truth; l = l->next) {
							const gchar *value = l->data;

							if (value && compare (value, argv[1]->value.string, region)) {
								truth = TRUE;
							} else if ((!value) && compare ("", argv[1]->value.string, region)) {
								truth = TRUE;
							}
						}
					}
				}
			}
		}
	}

	r = e_sexp_result_new (f, ESEXP_RES_BOOL);
	r->value.boolean = truth;

	return r;
}

static void
contains_helper_free_word (gpointer data,
                           gpointer user_data)
{
	if (data) {
		g_string_free ((GString *) data, TRUE);
	}
}

static gboolean
try_contains_word (const gchar *s1,
                   GSList *word)
{
	const gchar *o, *p;
	gunichar unival, first_w_char;
	GString *w;

	if (s1 == NULL)
		return FALSE;
	if (word == NULL)
		return TRUE; /* previous was last word */
	if (word->data == NULL)
		return FALSE; /* illegal structure */

	w = word->data;
	first_w_char = g_utf8_get_char (w->str);

	o = s1;
	for (p = e_util_unicode_get_utf8 (o, &unival); p && unival; p = e_util_unicode_get_utf8 (p, &unival)) {
		if (unival == first_w_char) {
			gunichar unival2;
			const gchar *q = p;
			const gchar *r = e_util_unicode_get_utf8 (w->str, &unival2);
			while (q && r && unival && unival2) {
				q = e_util_unicode_get_utf8 (q, &unival);
				if (!q)
					break;
				r = e_util_unicode_get_utf8 (r, &unival2);
				if (!r)
					break;
				if (unival != unival2)
					break;
			}
			if (!unival2 && r && q) {
				/* we read whole word and no illegal character has been found */
				if (word->next == NULL ||
				    try_contains_word (e_util_unicode_get_utf8 (o, &unival), word->next)) {
					return TRUE;
				}
			}
		}
		o = p;
	}

	return FALSE;
}

/* first space between words is treated as wildcard character;
 * we are looking for s2 in s1, so s2 will be breaked into words
*/
static gboolean
contains_helper (const gchar *s1,
                 const gchar *s2,
                 const gchar *region)
{
	gchar *s1uni;
	gchar *s2uni;
	GSList *words;
	gchar *next;
	gboolean have_nonspace;
	gboolean have_space;
	GString *last_word, *w;
	gboolean res;
	gunichar unich;
	glong len1, len2;

	if (!s2)
		return FALSE;

	/* the initial word contains an empty string for sure */
	if (!*s2)
		return TRUE;

	s1uni = e_util_utf8_normalize (s1);
	if (s1uni == NULL)
		return FALSE;

	s2uni = e_util_utf8_normalize (s2);
	if (s2uni == NULL) {
		g_free (s1uni);
		return FALSE;
	}

	len1 = g_utf8_strlen (s1uni, -1);
	len2 = g_utf8_strlen (s2uni, -1);
	if (len1 == 0 || len2 == 0) {
		g_free (s1uni);
		g_free (s2uni);

		/* both are empty strings */
		if (len1 == len2)
			return TRUE;

		return FALSE;
	}

	/* breaking s2 into words */
	words = NULL;
	have_nonspace = FALSE;
	have_space = FALSE;
	last_word = NULL;
	w = g_string_new ("");
	for (next = e_util_unicode_get_utf8 (s2uni, &unich); next && unich; next = e_util_unicode_get_utf8 (next, &unich)) {
		if (unich == ' ') {
			if (have_nonspace && !have_space) {
				/* treat only first space after nonspace character as wildcard,
				 * so we will start new word here
				*/
				have_space = TRUE;
				words = g_slist_append (words, w);
				last_word = w;
				w = g_string_new ("");
			} else {
				g_string_append_unichar (w, unich);
			}
		} else {
			have_nonspace = TRUE;
			have_space = FALSE;
			g_string_append_unichar (w, unich);
		}
	}

	if (have_space) {
		/* there was one or more spaces at the end of string,
		 * concat actual word with that last one
		*/
		g_string_append_len (last_word, w->str, w->len);
		g_string_free (w, TRUE);
	} else {
		/* append actual word into words list */
		words = g_slist_append (words, w);
	}

	res = try_contains_word (s1uni, words);

	g_free (s1uni);
	g_free (s2uni);
	g_slist_foreach (words, contains_helper_free_word, NULL);
	g_slist_free (words);

	return res;
}

static ESExpResult *
func_contains (struct _ESExp *f,
               gint argc,
               struct _ESExpResult **argv,
               gpointer data)
{
	SearchContext *ctx = data;

	return entry_compare (ctx, f, argc, argv, contains_helper);
}

static gboolean
is_helper (const gchar *ps1,
           const gchar *ps2,
           const gchar *region)
{
	gchar *s1, *s2;
	gboolean res = FALSE;

	s1 = e_util_utf8_remove_accents (ps1);
	s2 = e_util_utf8_remove_accents (ps2);

	if (!e_util_utf8_strcasecmp (s1, s2))
		res = TRUE;

	g_free (s1);
	g_free (s2);

	return res;
}

static ESExpResult *
func_is (struct _ESExp *f,
         gint argc,
         struct _ESExpResult **argv,
         gpointer data)
{
	SearchContext *ctx = data;

	return entry_compare (ctx, f, argc, argv, is_helper);
}

static gboolean
endswith_helper (const gchar *ps1,
                 const gchar *ps2,
                 const gchar *region)
{
	gchar *s1 = e_util_utf8_remove_accents (ps1);
	gchar *s2 = e_util_utf8_remove_accents (ps2);
	gboolean res = FALSE;
	glong s1len = g_utf8_strlen (s1, -1);
	glong s2len = g_utf8_strlen (s2, -1);

	if (s1len < s2len)
		res = FALSE;
	else
		res = e_util_utf8_strstrcase (g_utf8_offset_to_pointer (s1, s1len - s2len), s2) != NULL;

	g_free (s1);
	g_free (s2);

	return res;
}

static ESExpResult *
func_endswith (struct _ESExp *f,
               gint argc,
               struct _ESExpResult **argv,
               gpointer data)
{
	SearchContext *ctx = data;

	return entry_compare (ctx, f, argc, argv, endswith_helper);
}

static gboolean
beginswith_helper (const gchar *ps1,
                   const gchar *ps2,
                   const gchar *region)
{
	gchar *p;
	gboolean res = FALSE;
	gchar *s1 = e_util_utf8_remove_accents (ps1);
	gchar *s2 = e_util_utf8_remove_accents (ps2);

	if ((p = (gchar *) e_util_utf8_strstrcase (s1, s2))
	    && (p == s1))
		res = TRUE;

	g_free (s1);
	g_free (s2);

	return res;
}

static ESExpResult *
func_beginswith (struct _ESExp *f,
                 gint argc,
                 struct _ESExpResult **argv,
                 gpointer data)
{
	SearchContext *ctx = data;

	return entry_compare (ctx, f, argc, argv, beginswith_helper);
}

static gboolean
eqphone_helper (const gchar *ps1,
                const gchar *ps2,
                const gchar *region,
                EPhoneNumberMatch required_match)
{
	const EPhoneNumberMatch actual_match =
		e_phone_number_compare_strings_with_region (
			ps1, ps2, region, NULL);

	return actual_match >= E_PHONE_NUMBER_MATCH_EXACT
		&& actual_match <= required_match;
}

static gboolean
eqphone_exact_helper (const gchar *ps1,
                      const gchar *ps2,
                      const gchar *region)
{
	return eqphone_helper (ps1, ps2, region, E_PHONE_NUMBER_MATCH_EXACT);
}

static gboolean
eqphone_national_helper (const gchar *ps1,
                         const gchar *ps2,
                         const gchar *region)
{
	return eqphone_helper (ps1, ps2, region, E_PHONE_NUMBER_MATCH_NATIONAL);
}

static gboolean
eqphone_short_helper (const gchar *ps1,
                      const gchar *ps2,
                      const gchar *region)
{
	return eqphone_helper (ps1, ps2, region, E_PHONE_NUMBER_MATCH_SHORT);
}

static ESExpResult *
func_eqphone (struct _ESExp *f,
              gint argc,
              struct _ESExpResult **argv,
              gpointer data)
{
	SearchContext *ctx = data;

	return entry_compare (ctx, f, argc, argv, eqphone_exact_helper);
}

static ESExpResult *
func_eqphone_national (struct _ESExp *f,
                       gint argc,
                       struct _ESExpResult **argv,
                       gpointer data)
{
	SearchContext *ctx = data;

	return entry_compare (ctx, f, argc, argv, eqphone_national_helper);
}

static ESExpResult *
func_eqphone_short (struct _ESExp *f,
                    gint argc,
                    struct _ESExpResult **argv,
                    gpointer data)
{
	SearchContext *ctx = data;

	return entry_compare (ctx, f, argc, argv, eqphone_short_helper);
}

static gboolean
regex_helper (const gchar *ps1,
              const gchar *ps2,
              const gchar *region,
              gboolean normalize)
{
	const gchar *field_data = ps1;
	const gchar *expression = ps2;
	GRegex      *regex;
	GError      *error = NULL;
	gboolean     match = FALSE;

	regex = g_regex_new (expression, 0, 0, &error);
	if (!regex) {
		g_warning (
			"Failed to parse regular expression '%s': %s",
			expression, error ? error->message : _("Unknown error"));
		g_clear_error (&error);
		return FALSE;
	}

	if (normalize) {
		gchar *normal = e_util_utf8_normalize (field_data);

		match = g_regex_match (regex, normal, 0, NULL);

		g_free (normal);
	} else
		match = g_regex_match (regex, field_data, 0, NULL);

	g_regex_unref (regex);

	return match;
}

static gboolean
regex_normal_helper (const gchar *ps1,
                     const gchar *ps2,
                     const gchar *region)
{
	return regex_helper (ps1, ps2, region, TRUE);
}

static gboolean
regex_raw_helper (const gchar *ps1,
                  const gchar *ps2,
                  const gchar *region)
{
	return regex_helper (ps1, ps2, region, FALSE);
}

static ESExpResult *
func_regex_normal (struct _ESExp *f,
                   gint argc,
                   struct _ESExpResult **argv,
                   gpointer data)
{
	SearchContext *ctx = data;

	return entry_compare (ctx, f, argc, argv, regex_normal_helper);
}

static ESExpResult *
func_regex_raw (struct _ESExp *f,
                gint argc,
                struct _ESExpResult **argv,
                gpointer data)
{
	SearchContext *ctx = data;

	return entry_compare (ctx, f, argc, argv, regex_raw_helper);
}

static gboolean
exists_helper (const gchar *ps1,
               const gchar *ps2,
               const gchar *region)
{
	gboolean res = FALSE;
	gchar *s1 = e_util_utf8_remove_accents (ps1);
	gchar *s2 = e_util_utf8_remove_accents (ps2);

	if (e_util_utf8_strstrcase (s1, s2))
		res = TRUE;

	g_free (s1);
	g_free (s2);

	return res;
}

static ESExpResult *
func_exists (struct _ESExp *f,
             gint argc,
             struct _ESExpResult **argv,
             gpointer data)
{
	SearchContext *ctx = data;
	ESExpResult *r;
	gint truth = FALSE;

	if (argc == 1
	    && argv[0]->type == ESEXP_RES_STRING) {
		const gchar *propname;
		struct prop_info *info = NULL;
		gint i;
		gboolean saw_any = FALSE;

		propname = argv[0]->value.string;

		for (i = 0; i < G_N_ELEMENTS (prop_info_table); i++) {
			if (!strcmp (prop_info_table[i].query_prop, propname)) {
				saw_any = TRUE;
				info = &prop_info_table[i];

				if (info->prop_type == PROP_TYPE_NORMAL) {
					const gchar *prop = NULL;
					/* searches where the query's property
					 * maps directly to an ecard property */

					prop = e_contact_get_const (ctx->contact, info->field_id);

					if (prop && *prop)
						truth = TRUE;
				}
				else if (info->prop_type == PROP_TYPE_LIST) {
					/* the special searches that match any of the list elements */
					truth = info->list_compare (ctx->contact, "", NULL, exists_helper);
				}
				else if (info->prop_type == PROP_TYPE_DATE) {
					EContactDate *date;

					date = e_contact_get (ctx->contact, info->field_id);

					if (date) {
						truth = TRUE;
						e_contact_date_free (date);
					}
				} else {
					g_warn_if_reached ();

					saw_any = FALSE;
				}

				break;
			}
		}

		if (!saw_any) {
			/* propname didn't match to any of our known "special" properties,
			 * so try to find if it isn't a real field and if so, then check
			 * against value in this field only */
			EContactField fid = e_contact_field_id (propname);

			if (fid >= E_CONTACT_FIELD_FIRST && fid < E_CONTACT_FIELD_LAST &&
			    e_contact_field_is_string (fid)) {
				const gchar *prop = e_contact_get_const (ctx->contact, fid);

				if (prop && *prop)
					truth = TRUE;
			} else {
				/* is is not a known EContact field, try with EVCard attributes */
				EVCardAttribute *attr;
				GList *l, *values;

				if (fid >= E_CONTACT_FIELD_FIRST && fid < E_CONTACT_FIELD_LAST)
					propname = e_contact_vcard_attribute (fid);

				attr = e_vcard_get_attribute (E_VCARD (ctx->contact), propname);
				values = attr ? e_vcard_attribute_get_values (attr) : NULL;

				for (l = values; l && !truth; l = l->next) {
					const gchar *value = l->data;

					if (value && *value)
						truth = TRUE;
				}
			}
		}
	}
	r = e_sexp_result_new (f, ESEXP_RES_BOOL);
	r->value.boolean = truth;

	return r;
}

static ESExpResult *
func_exists_vcard (struct _ESExp *f,
                   gint argc,
                   struct _ESExpResult **argv,
                   gpointer data)
{
	SearchContext *ctx = data;
	ESExpResult *r;
	gint truth = FALSE;

	if (argc == 1 && argv[0]->type == ESEXP_RES_STRING) {
		const gchar *attr_name;
		EVCardAttribute *attr;
		GList *values;
		gchar *s;

		attr_name = argv[0]->value.string;
		attr = e_vcard_get_attribute (E_VCARD (ctx->contact), attr_name);
		if (attr) {
			values = e_vcard_attribute_get_values (attr);
			if (g_list_length (values) > 0) {
				s = values->data;
				if (s[0] != '\0') {
					truth = TRUE;
				}
			}
		}
	}

	r = e_sexp_result_new (f, ESEXP_RES_BOOL);
	r->value.boolean = truth;

	return r;
}

static void
book_backend_sexp_finalize (GObject *object)
{
	EBookBackendSExpPrivate *priv;

	priv = E_BOOK_BACKEND_SEXP_GET_PRIVATE (object);

	g_object_unref (priv->search_sexp);
	g_free (priv->text);
	g_free (priv->search_context);

	/* Chain up to parent's finalize() method. */
	G_OBJECT_CLASS (e_book_backend_sexp_parent_class)->finalize (object);
}

static void
e_book_backend_sexp_class_init (EBookBackendSExpClass *class)
{
	GObjectClass *object_class;

	g_type_class_add_private (class, sizeof (EBookBackendSExpPrivate));

	object_class = G_OBJECT_CLASS (class);
	object_class->finalize = book_backend_sexp_finalize;
}

static void
e_book_backend_sexp_init (EBookBackendSExp *sexp)
{
	sexp->priv = E_BOOK_BACKEND_SEXP_GET_PRIVATE (sexp);
	sexp->priv->search_context = g_new (SearchContext, 1);
}

/* 'builtin' functions */
static struct {
	const gchar *name;
	ESExpFunc *func;
	gint type;	/* 1 if a function can perform shortcut evaluation,
			 * or doesn't execute everything, 0 otherwise */
} symbols[] = {
	{ "contains", func_contains, 0 },
	{ "is", func_is, 0 },
	{ "beginswith", func_beginswith, 0 },
	{ "endswith", func_endswith, 0 },
	{ "eqphone", func_eqphone, 0 },
	{ "eqphone_national", func_eqphone_national, 0 },
	{ "eqphone_short", func_eqphone_short, 0 },
	{ "regex_normal", func_regex_normal, 0 },
	{ "regex_raw", func_regex_raw, 0 },
	{ "exists", func_exists, 0 },
	{ "exists_vcard", func_exists_vcard, 0 },
};

/**
 * e_book_backend_sexp_new:
 * @text: an s-expression to parse
 *
 * Creates a new #EBookBackendSExp from @text.
 *
 * Returns: a new #EBookBackendSExp
 **/
EBookBackendSExp *
e_book_backend_sexp_new (const gchar *text)
{
	EBookBackendSExp *sexp;
	gint ii;

	g_return_val_if_fail (text != NULL, NULL);

	sexp = g_object_new (E_TYPE_BOOK_BACKEND_SEXP, NULL);
	sexp->priv->search_sexp = e_sexp_new ();
	sexp->priv->text = g_strdup (text);

	for (ii = 0; ii < G_N_ELEMENTS (symbols); ii++) {
		if (symbols[ii].type == 1) {
			e_sexp_add_ifunction (
				sexp->priv->search_sexp, 0,
				symbols[ii].name,
				(ESExpIFunc *) symbols[ii].func,
				sexp->priv->search_context);
		} else {
			e_sexp_add_function (
				sexp->priv->search_sexp, 0,
				symbols[ii].name,
				symbols[ii].func,
				sexp->priv->search_context);
		}
	}

	e_sexp_input_text (sexp->priv->search_sexp, text, strlen (text));

	if (e_sexp_parse (sexp->priv->search_sexp) == -1) {
		g_warning (
			"%s: Error in parsing: %s",
			G_STRFUNC, e_sexp_get_error (sexp->priv->search_sexp));
		g_object_unref (sexp);
		sexp = NULL;
	}

	return sexp;
}

/**
 * e_book_backend_sexp_text:
 * @sexp: an #EBookBackendSExp
 *
 * Retrieve the text expression for the given #EBookBackendSExp object.
 *
 * Returns: the text expression
 *
 * Since: 3.8
 **/
const gchar *
e_book_backend_sexp_text (EBookBackendSExp *sexp)
{
	g_return_val_if_fail (E_IS_BOOK_BACKEND_SEXP (sexp), NULL);

	return sexp->priv->text;
}

/**
 * e_book_backend_sexp_match_contact:
 * @sexp: an #EBookBackendSExp
 * @contact: an #EContact
 *
 * Checks if @contact matches @sexp.
 *
 * Returns: %TRUE if the contact matches, %FALSE otherwise
 **/
gboolean
e_book_backend_sexp_match_contact (EBookBackendSExp *sexp,
                                   EContact *contact)
{
	ESExpResult *r;
	gboolean retval;

	g_return_val_if_fail (E_IS_BOOK_BACKEND_SEXP (sexp), FALSE);
	g_return_val_if_fail (E_IS_CONTACT (contact), FALSE);

	sexp->priv->search_context->contact = g_object_ref (contact);

	r = e_sexp_eval (sexp->priv->search_sexp);

	retval = (r && r->type == ESEXP_RES_BOOL && r->value.boolean);

	g_object_unref (sexp->priv->search_context->contact);

	e_sexp_result_free (sexp->priv->search_sexp, r);

	return retval;
}

/**
 * e_book_backend_sexp_match_vcard:
 * @sexp: an #EBookBackendSExp
 * @vcard: a vCard string
 *
 * Checks if @vcard matches @sexp.
 *
 * Returns: %TRUE if the vCard matches, %FALSE otherwise
 **/
gboolean
e_book_backend_sexp_match_vcard (EBookBackendSExp *sexp,
                                 const gchar *vcard)
{
	EContact *contact;
	gboolean retval;

	g_return_val_if_fail (E_IS_BOOK_BACKEND_SEXP (sexp), FALSE);
	g_return_val_if_fail (vcard != NULL, FALSE);

	contact = e_contact_new_from_vcard (vcard);

	retval = e_book_backend_sexp_match_contact (sexp, contact);

	g_object_unref (contact);

	return retval;
}

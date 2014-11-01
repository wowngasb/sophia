
/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

#include <libsr.h>
#include <libst.h>
#include <sophia.h>

static void
ctl_version(stc *cx srunused)
{
	void *env = sp_env();
	t( env != NULL );

	void *c = sp_ctl(env);
	t( c != NULL );

	void *o = sp_get(c, "sophia.version");
	t( o != NULL );
	t( strcmp(sp_get(o, "value", NULL), "1.2") == 0 );
	sp_destroy(o);

	t( sp_destroy(env) == 0 );
}

static void
ctl_error_injection(stc *cx srunused)
{
	void *env = sp_env();
	t( env != NULL );

	void *c = sp_ctl(env);
	t( c != NULL );
	t( sp_set(c, "db.test") == 0 );

	void *o = sp_get(c, "db.test.error_injection.si_branch_0");
	t( o != NULL );
	t( strcmp(sp_get(o, "value", NULL), "0") == 0 );
	sp_destroy(o);

	t( sp_set(c, "db.test.error_injection.si_branch_0", "1") == 0 );
	o = sp_get(c, "db.test.error_injection.si_branch_0");
	t( o != NULL );
	t( strcmp(sp_get(o, "value", NULL), "1") == 0 );
	sp_destroy(o);

	t( sp_destroy(env) == 0 );
}

static void
ctl_scheduler(stc *cx srunused)
{
	void *env = sp_env();
	t( env != NULL );

	void *c = sp_ctl(env);
	t( c != NULL );
	t( sp_set(c, "scheduler.threads", "2") == 0 );

	void *o = sp_get(c, "scheduler.0.trace");
	t( o == NULL );
	t( sp_open(env) == 0 );

	o = sp_get(c, "scheduler.0.trace");
	t( o != NULL );
	t( strcmp(sp_get(o, "value", NULL), "init") == 0 ||
	   strcmp(sp_get(o, "value", NULL), "sleep") == 0 ||
	   strcmp(sp_get(o, "value", NULL), "schedule") == 0 ||
	   strcmp(sp_get(o, "value", NULL), "log gc") == 0 );
	sp_destroy(o);

	o = sp_get(c, "scheduler.1.trace");
	t( o != NULL );
	t( strcmp(sp_get(o, "value", NULL), "init") == 0 ||
	   strcmp(sp_get(o, "value", NULL), "sleep") == 0 ||
	   strcmp(sp_get(o, "value", NULL), "schedule") == 0 ||
	   strcmp(sp_get(o, "value", NULL), "log gc") == 0 );
	sp_destroy(o);

	o = sp_get(c, "scheduler.2.trace");
	t( o == NULL );

	t( sp_destroy(env) == 0 );
}

static void
ctl_cursor(stc *cx srunused)
{
	void *env = sp_env();
	t( env != NULL );
	void *c = sp_ctl(env);
	t( c != NULL );
	t( sp_set(c, "db.test") == 0 );
	void *o = sp_get(c, "db.test.branch");
	t( o != NULL );
	sp_destroy(o);
	void *cur = sp_cursor(c, ">=", NULL);
	t( cur != NULL );
	printf("\n");
	while ((o = sp_get(cur))) {
		char *key = sp_get(o, "key", NULL);
		char *value = sp_get(o, "value", NULL);
		printf("%s", key);
		if (value)
			printf(" = %s\n", value);
		else
			printf(" = \n");
	}
	t( sp_destroy(cur) == 0 );
	t( sp_destroy(env) == 0 );
}

stgroup *ctl_group(void)
{
	stgroup *group = st_group("ctl");
	st_groupadd(group, st_test("version", ctl_version));
	st_groupadd(group, st_test("error_injection", ctl_error_injection));
	st_groupadd(group, st_test("scheduler", ctl_scheduler));
	st_groupadd(group, st_test("cursor", ctl_cursor));
	return group;
}

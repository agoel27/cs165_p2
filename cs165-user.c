#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "cs165-user.h"

system_t users;
FILE *ifile;
FILE *ofile;


/******************************************************************************

Task 2: create a new user

Function: system_user_new
Input Args:
          user_index_str - string for the user index value for the new user
          name - name string for the user
Returns: a new user object or NULL

system_user_new creates a new user of index <user_index> and name <name>.

NOTE: assumes there is no user with this index already

*******************************************************************************/

extern int free_slot( user_t *user_array[] );

user_t *system_user_new( char *user_index, char *user_type, char *user_input )
{
  int index = atoi(user_index);
  int type = atoi(user_type);
  student_t *s;
  anon_t *a;
  spy_t *m;
  user_t *new_user;
  int id;

  if ( find_user( user_index )) return (user_t *)NULL;
  
  switch(type) {
  case STUDENT_USER:
    s = (student_t *) malloc(sizeof(student_t));
    student_new(s, user_input);
    new_user = (user_t *)s;
    break;
  case ANON_USER:
    a = (anon_t *) malloc(sizeof(anon_t));
    id = atoi(user_input);
    anon_new(a, id);
    new_user = (user_t *)a;
    break;
  case SPY_USER:
    m = (spy_t *) malloc(sizeof(spy_t));
    id = atoi(user_input);
    spy_new(m, id);
    new_user = (user_t *)m;
    break;
  default:
    return (user_t *)NULL;
    break;
  }

  int slot = free_slot(users.members);
  if ( slot < 0 ) {
    free( new_user );
    return (user_t *)NULL;  // no free slot to add a new user
  }
  users.members[slot] = new_user;
  new_user->index = index;
  new_user->type = type;

  return new_user;
}


int free_slot( user_t *user_array[] )
{
  for ( int i = 0; i < MAX_USERS; i++ ) {
    if ( users.members[i] == NULL ) {
      return i;
    }
  }

  return -1;
}


/******************************************************************************

High-level Command Processing Functions: provided

*******************************************************************************/

/******************************************************************************

Function: find_command
Input Args:
          cmdbuf - buffer for the command line from the input file
          len - length of the cmdbuf
Returns: a command index or an error value (<0)
 
find_command determines the specific command and checks the argument
count.

*******************************************************************************/

int find_command( char *cmdbuf, int len )
{
  int ct = strct( cmdbuf, len );

  switch( cmdbuf[0] ) {
  case 'u':   // new user 
    if ( ct != 4 ) {
      return -1;
    }
    return USER_NEW;
    break;
  case 'a':   // add user-specific info
    if ( ct != 4 ) {
      return -1;
    }
    return USER_ADD;
    break;
  case 'r':   // remove
    if ( ct != 2 ) {
      return -1;
    }
    return USER_REMOVE;
    break;
  case 'c':   // change
    if ( ct != 4 ) {
      return -1;
    }
    return USER_CHANGE;
    break;
  case 'j':   // join group 
    if ( ct != 3 ) {
      return -1;
    }
    return USER_JOIN;
    break;
  case 'l':   // leave group 
    if ( ct != 2 ) {
      return -1;
    }
    return USER_LEAVE;
    break;
  case 'd':   // display
    if ( ct != 2 ) {
      return -1;
    }
    return USER_DISPLAY;
    break;
  case 'g':   // new group
    if ( ct != 2 ) {
      return -1;
    }
    return GROUP_NEW;
    break;
  case 'p':   // delete (purge) group
    if ( ct != 2 ) {
      return -1;
    }
    return GROUP_PURGE;
    break;
  case 's':   // group display members
    if ( ct != 2 ) {
      return -1;
    }
    return GROUP_SHOW;
    break;
  case '#':   // comment lines
  case '%':
  case '\n':
    return SYSTEM_SKIP;
    break;
  }

  return -1;
}


/******************************************************************************

Function: find_user
Input Args:
          user_index_str - the index as a string
          cmd - command number
Returns: a user reference or NULL
        
find_user find the user correpsonding to the index value from
<user_index_str>, if one exists.

*******************************************************************************/

user_t *find_user( char *user_index_str )
{
  int user_index = atoi( user_index_str );  
  int i;

  if ( user_index > 0 ) {
    for ( i = 0; i < MAX_USERS; i++ ) {
      user_t *test_user = users.members[i];
      if (( test_user ) && ( test_user->index == user_index )) {
	return test_user;
      }
    }
  }

  //  fprintf( ofile, "ERR: Unknown user: %s\n", user_index_str );
  return (user_t *) NULL;
}


/******************************************************************************

Function: apply_command
Input Args:
          user - user object (may be NULL for USER_NEW)
          cmd - command number
          args - arguments string for the command
Returns: 0 on success and <0 on error
 
Run the respective command on the user object with the exception of
USER_NEW, which creates a user object.

*******************************************************************************/


int apply_command( user_t *user, int cmd, char *args )
{
  char arg1[MAX_STRING], arg2[MAX_STRING], arg3[MAX_STRING];
  int res = 0;
  int type = 0;

#if 0
  student_t *s = NULL;
  anon_t *a = NULL;
  spy_t *m = NULL;

  if ( user ) {
    switch (user->type) {
    case STUDENT_USER:
      s = (student_t *)user;
      break;
    case ANON_USER:
      a = (anon_t *)user;
      break;
    case SPY_USER:
      m = (spy_t *)user;
      break;
    default:
      return -1;
    }
  }
#endif
  
  
  switch( cmd ) {
  case USER_NEW:
    if (( sscanf( args, "%s %s %s", arg1, arg2, arg3 )) != 3 ) {
	return -1;
    }
    user = system_user_new( arg1, arg2, arg3 );
    if ( user ) {
      fprintf( ofile, "USER_NEW for index %s type %s and name/id %s: result %d\n", arg1, arg2, arg3, res );
      return 0;
    }
    else {
      return -1;
    }
    break;
  case USER_ADD:
    if (( sscanf( args, "%s %s", arg1, arg2 )) != 2 ) {
      return -1;
    }
    type = atoi(arg1);
    res = USER_FN_ADD( user, type, arg2 );
    fprintf( ofile, "USER_ADD for user %d and type %s to add info %s: result %d\n", user->index, arg1, arg2, res );
    return res;
    break;
  case USER_REMOVE:
    fprintf( ofile, "USER_REMOVE for user %d(%d) : result %d\n", user->index, user->type, res );
    res = user_remove( user );
    if ( res < 0 ) {
      fprintf( ofile, "USER_REMOVE failure : result %d\n", res );
    }
    return res;
    break;
  case USER_CHANGE:
    if (( sscanf( args, "%s %s", arg1, arg2 )) != 2 ) {
	return -1;
    }
    type = atoi(arg1);
    res = USER_FN_CHANGE( user, type, arg2 );
    fprintf( ofile, "USER_CHANGE for user %d and type %s to add secret %s : result %d\n", user->index, arg1, arg2, res );
    return res;
    break;
  case USER_JOIN:
    if (( sscanf( args, "%s", arg1 )) != 1 ) {
	return -1;
    }
    res = USER_FN_JOIN( user, arg1 );
    fprintf( ofile, "USER_JOIN for user %d(%d) to join group %s : result %d\n", user->index, user->type, arg1, res );
    return res;
    break;
  case USER_LEAVE:
    res = USER_FN_LEAVE( user );
    fprintf( ofile, "USER_LEAVE for user %d(%d) to leave group %d: result %d\n", user->index, user->type, ( (user->group) ? (user->group->index) : -1) , res );
    return res;
    break;
  case USER_DISPLAY:
    res = USER_FN_DISPLAY( user );
    fprintf( ofile, "USER_DISPLAY for user %d(%d) : result %d\n", user->index, user->type, res );
    return res;
    break;
  default:
    fprintf( ofile, "Err: apply_command: Invalid command\n");
    return -1;
    break;
  }
}

int mynoop()
{
  printf("ERR: running in noop - some kind of bug in the user object\n");
  return 0;
}


  
/*********************************
  
  Task functions - see function prototypes in cs165-user.h

**********************************/

int student_new( student_t *s, char *name ) 
{
  strncpy( s->name, name, MAX_STRING );
  s->add = student_add;
  s->remove = student_remove;
  s->change = student_change;
  s->join = student_join;
  s->leave = student_leave;
  s->display = student_display;
  
  return 0;
}


int student_add( char *secret, student_t *s )
{
  strncpy( s->secret, secret, MAX_STRING );

  return 0;
}


int student_remove( student_t *s )
{
  return user_remove((user_t *) s);
}


int user_remove( user_t *u )
{
  // remove from the system
  for ( int i = 0; i < MAX_USERS; i++ ) {
    if (( users.members[i] ) && ( u->index == users.members[i]->index)) {
      users.members[i] = (user_t *) NULL;
      break;
    }
  }

  // deallocate mem
  free( u );

  return 0;
}


int student_change( student_t *s, char *name )
{
  strncpy( s->name, name, MAX_STRING );

  return 0;
}


int student_join( student_t *s, char *group_index_str )
{
  group_t *g = find_group( group_index_str );

  if ( g ) {
    group_add( g, (user_t *)s );
    s->group = g;
    return 0;
  }

  return -1;
}


int student_leave( student_t *s )
{
  group_t *g = s->group;
  
  if ( g ) {
    group_remove( g, (user_t *)s );
    return 0;
  }

  return -1;
}


int student_display( student_t *s )
{
  fprintf( ofile, "****** student (%d): %s *******\n", s->index, s->name );

  return 0;
}
 //  fprintf( ofile, "-----> belongs to group: %d <------\n", s->group->index );


int user_display( user_t *u )
{
  switch( u->type ) {
  case STUDENT_USER:
    ((student_t *)u)->display((student_t *)u);
    break;
  case ANON_USER:
    ((anon_t *)u)->display((anon_t *)u);
    break;
  case SPY_USER:
    ((spy_t *)u)->display((spy_t *)u);
    break;
  default:
    printf("No display for user %d\n", u->index);
  }

  return 0;
}


int anon_new( anon_t *a, int id ) 
{
  a->id = id;
  a->add = anon_add;
  a->remove = anon_remove;
  a->change = anon_change;
  a->join = anon_join;
  a->leave = anon_leave;
  a->display = anon_display;

  return 0;
}

int anon_add( char *arg, anon_t *a ) { return 0; }


int anon_change( anon_t *a, char *id_str )
{
  int id = atoi( id_str );

  a->id = id;
  
  return 0;
}


int anon_remove( anon_t *a )
{
  return user_remove((user_t *) a);
}


int anon_join( anon_t *a, char *group_index )
{
  return student_join((student_t *) a, group_index);
}


int anon_leave( anon_t *a )
{
  return student_leave((student_t *) a);
}


int anon_display( anon_t *a )
{
  fprintf( ofile, "****** anonymous (%d): %d *******\n", a->index, a->id );
  
  return 0;
}
  //  fprintf( ofile, "-----> belongs to group: %d <------\n", a->group->index );


int spy_new( spy_t *m, int id ) 
{
  m->id = id;
  m->add = spy_add;
  m->remove = spy_remove;
  m->change = spy_change;
  m->join = spy_join;
  m->leave = spy_leave;
  m->display = spy_display;

  return 0;
}


int spy_add( char *arg, spy_t *m )
{
  user_t *u = find_user( arg );  // is the real_index for the user

  if ( u ) {
    m->real = u;
    return 0;
  }

  return -1;
}


int spy_remove( spy_t *m )
{
  return user_remove((user_t *) m);
}



int spy_change( spy_t *m, char *real_index_str )
{
  user_t *u = find_user( real_index_str );

  m->real = u;
  
  return 0;
}


int spy_join( spy_t *m, char *group_index )
{
  return student_join((student_t *) m, group_index);
}


int spy_leave( spy_t *m )
{
  return student_leave((student_t *) m);
}


int spy_display( spy_t *m )
{
  fprintf( ofile, "****** spy (%d)  *******\n", m->index );
  
  return 0;
}

  // fprintf( ofile, "-----> belongs to group: %d <------\n", m->group->index );



/*********************************
  
  group commands

**********************************/

group_t *find_group( char *group_index )
{
  int gindex = atoi( group_index );

  list_t *l = (list_t *)users.groups;

  if ( !l ) return (group_t *)NULL;

  elt_t *e = l->head;
  while ( e ) {
    group_t *g = (group_t *) e->obj;

    if (( g ) && ( g->index == gindex )) {
      return g;
    }
    e = e->next;    
  }

  fprintf( ofile, "ERR: Unknown group: %s\n", group_index );
  return (group_t *)NULL;
}


extern int list_add( list_t *l, void *obj );
extern int list_remove( list_t *l, void *obj );

int group_add( group_t *g, user_t *u )
{
  list_t *members = g->members;
  
  int res = list_add( members, (void *)u );

  return res;
}


int list_add( list_t *l, void *obj )
{
  elt_t *e = (elt_t *)malloc(sizeof(elt_t));

  if ( !l ) return -1;
  
  e->obj = obj;
  if ( l->head ) l->head->prev = e;
  e->next = l->head;
  e->prev = (elt_t *)NULL;
  l->head = e;

  return 0;  
}


int group_remove( group_t *g, user_t *u )
{
  list_t *members = g->members;

  int res = list_remove(members, u);

  return res;  // u is not a member of the group
}


int list_remove( list_t *l, void *obj )
{
  elt_t *e = l->head;

  while ( e ) {
    if ( e->obj == (void *)obj ) {
      if ( e->prev == NULL ) {  // head case
	l->head = e->next;
	if ( l->head )
	  l->head->prev = NULL;
      }
      else {
	elt_t *next = e->next;
	elt_t *prev = e->prev;

	if (next) next->prev = prev;
	prev->next = next;
      }
      free( e );
      return 0;
    }
    e = e->next;
  }

  return -1;  // didn't find elt with that obj
}


int group_purge( group_t *g )
{
  list_t *members = g->members;
  elt_t *e = members->head;

  if ( !e ) return 0;
  
  while ( e ) {
    user_t *u = (user_t *)e->obj;
    u->group = (group_t *) NULL;
    elt_t *prev_e = e;
    e = e->next;
    free( prev_e );
  }
  list_remove( users.groups, g );
  free( members );
  free( g );
  
  return 0;
}


int group_show( group_t *g )
{
  fprintf( ofile, "\n======== group: %d ========\n\n", g->index );
  
  list_t *members = g->members;

  if ( !members ) return -1;
  
  elt_t *e = members->head;
  while ( e ) {
    user_t *u = (user_t *)e->obj;
    user_display( u );
    e = e->next;
  }

  fprintf( ofile, "\n");
  
  return 0;
}



/******************************************************************************

Function: apply_user_command
Input Args:
          cmd - command number
          cmdstr - string for command
Returns: 0 on success and <0 on error
 
Prepares the command string (cmdstr) into individual arguments (args)
to run the command on user objects in apply_command.

*******************************************************************************/


int apply_user_command( int cmd, char *cmdstr )
{
  user_t *user = NULL;
  char user_index_str[MAX_STRING];  
  char *args;
  int res = 0;
  
  args = cmdstr;

  if ( cmd != USER_NEW ) {
    sscanf( cmdstr, "%s", user_index_str );  
    user = find_user( user_index_str );  

    // skip over user_index_str arg
    while (( (++args)[0] != ' ' ) && (args[0] != '\n'));
    args++;

    if ( user == NULL ) {
      return -1;
    }
  }

  // apply the command
	  
  res = apply_command( user, cmd, args );

  if ( res < 0 ) {
    fprintf( ofile, "ERR: failed command: cmd %d for line %s\n", cmd, cmdstr );
  }

  return res;
}


/******************************************************************************

Function: apply_group_command
Input Args:
          cmd - command number
          cmdstr - string for command
Returns: 0 on success and <0 on error
 
Prepares the command string (cmdstr) to run the respective command (cmd) on
the group object.

*******************************************************************************/

int apply_group_command( int cmd, char *cmdstr )
{
  group_t *group = NULL;
  char group_index_str[MAX_STRING];  
  int res = 0;
  
  sscanf( cmdstr, "%s", group_index_str );  
  if ( cmd != GROUP_NEW ) {
    group = find_group( group_index_str );  

    if ( group == NULL ) {
      return -1;
    }
  }

  // apply the command
	  
  switch( cmd ) {
  case GROUP_NEW:
    group = (group_t *)malloc(sizeof(group_t));
    group->index = atoi(group_index_str);
    group->members = (list_t *)malloc(sizeof(list_t));
    group->members->head = (elt_t *)NULL;
    list_add( users.groups, (void *)group );
    fprintf( ofile, "GROUP_NEW: added new group %s\n", group_index_str );
    break;
  case GROUP_PURGE:
    fprintf( ofile, "GROUP_PURGE: removing group %d: res: %d\n", group->index, res );
    res = group_purge( group );
    break;
  case GROUP_SHOW:
    fprintf( ofile, "GROUP_SHOW: showing group %d: res: %d\n", group->index, res );
    res = group_show( group );
    return res;
    break;
  }

  if ( res < 0 ) {
    fprintf( ofile, "ERR: failed command: cmd %d for group %s for line %s\n", cmd, group_index_str, cmdstr );
  }

  return res;
}


/******************************************************************************

Function: main

Open the input and output files
Obtain the command lines one-by-one from the input file
Determine the command to run in "find_command"
Run the command in one of the "apply_command" functions

Task 1: complete the input processing in main

*******************************************************************************/



int main( int argc, char *argv[] )
{
	FILE *file;
	int cmd;
	int res = 0;
	size_t len = MAX_LINE;
	char *line = NULL;
	char *cmdstr;


	// initialize system
	users.groups = (list_t *)malloc(sizeof(list_t));
	users.groups->head = (elt_t *)NULL;
	memset( users.members, 0, sizeof(user_t *) * MAX_USERS );
	
	// check usage
	if ( argc != 3 ) {
//	  printf( "Main: Err: Incorrect usage: %s\n", USAGE );
	  system("echo \"Main: Err: Incorrect usage\"");
	}

	// open fds
	if (( file = fopen( argv[1], "r" )) == NULL ) {
	  exit(-1);
	}

	ifile = file;

	if (( file = fopen( argv[2], "w+" )) == NULL ) {
          fscanf( ifile, "%[^\n]", line);
	  exit(-1);
	}

	ofile = file;
	 
	// get input
	while (( res = getline( &line, &len, ifile )) >= 0 )
	// while (( res = fscanf( ifile, " %[^\n]", line ) >= 0))
	  {
	  
	  cmdstr = line;
	  // determine command from command string (first arg in line)

	  cmd = find_command( cmdstr, strlen( cmdstr ));
	  if ( cmd == SYSTEM_SKIP ) {
	    continue;
	  }	  
	  if ( cmd < 0 ) {
	    fprintf( ofile, "ERR: invalid command line: %s", line );
	    continue;
	  }

	  // TBD (2): get reference to args from cmdstr
	  
	  while ( (++cmdstr)[0] != ' ' );
	  cmdstr++;

	  if ( cmd >= GROUP_NEW ) { // if a group command
	    res = apply_group_command( cmd, cmdstr );
	  }
	  else {
	    res = apply_user_command( cmd, cmdstr );
	  }
	}

//	exit( 0 );
	return 0;
}




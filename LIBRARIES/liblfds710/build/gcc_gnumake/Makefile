##### notes #####
# TRD : -fno-strict-aliasing is needed because GCC has messed up type punning and __may_alias__ does absolutely nothing
#       -Wno-unused-but-set-variable and -Wno-uninitialized are needed because GCC seems confused by the atomic intrinsics
#       the code base for release has been compiled with those warnings enabled, to show any valid errors

##### paths #####
LIBDIR    := ../../..
BINDIR    := ../../bin
INCDIR    := ../../inc
OBJDIR    := ../../obj
SRCDIR    := ../../src
INSINCDIR := /usr/include
INSLIBDIR := /usr/lib

##### misc #####
QUIETLY        := 1>/dev/null 2>/dev/null
VERSION_NUMBER := 1
MINOR_NUMBER   := 0
RELEASE_NUMBER := 0

##### sources, objects and libraries #####
BINNAME    := liblfds710
ARFILENAME := $(BINNAME).a
ARPATHNAME := $(BINDIR)/$(ARFILENAME)
SOBASENAME := $(BINNAME).so
SONAME     := $(SOBASENAME).$(VERSION_NUMBER)
SOFILENAME := $(SONAME).$(MINOR_NUMBER).$(RELEASE_NUMBER)
SOPATHNAME := $(BINDIR)/$(SOFILENAME)
INCNAME    := $(INCDIR)/$(BINNAME).h
SRCDIRS    := lfds710_btree_addonly_unbalanced lfds710_freelist lfds710_hash_addonly lfds710_list_addonly_singlylinked_ordered lfds710_list_addonly_singlylinked_unordered lfds710_misc lfds710_prng lfds710_queue_bounded_manyproducer_manyconsumer lfds710_queue_bounded_singleproducer_singleconsumer lfds710_queue_unbounded_manyproducer_manyconsumer lfds710_ringbuffer lfds710_stack
SOURCES    := lfds710_hash_addonly_cleanup.c lfds710_hash_addonly_get.c lfds710_hash_addonly_init.c lfds710_hash_addonly_insert.c lfds710_hash_addonly_iterate.c lfds710_hash_addonly_query.c \
              lfds710_list_addonly_singlylinked_ordered_cleanup.c lfds710_list_addonly_singlylinked_ordered_get.c lfds710_list_addonly_singlylinked_ordered_init.c lfds710_list_addonly_singlylinked_ordered_insert.c lfds710_list_addonly_singlylinked_ordered_query.c \
              lfds710_list_addonly_singlylinked_unordered_cleanup.c lfds710_list_addonly_singlylinked_unordered_get.c lfds710_list_addonly_singlylinked_unordered_init.c lfds710_list_addonly_singlylinked_unordered_insert.c lfds710_list_addonly_singlylinked_unordered_query.c \
              lfds710_btree_addonly_unbalanced_cleanup.c lfds710_btree_addonly_unbalanced_get.c lfds710_btree_addonly_unbalanced_init.c lfds710_btree_addonly_unbalanced_insert.c lfds710_btree_addonly_unbalanced_query.c \
              lfds710_freelist_cleanup.c lfds710_freelist_init.c lfds710_freelist_pop.c lfds710_freelist_push.c lfds710_freelist_query.c \
              lfds710_misc_internal_backoff_init.c lfds710_misc_globals.c lfds710_misc_query.c \
              lfds710_prng_init.c \
              lfds710_queue_bounded_manyproducer_manyconsumer_cleanup.c lfds710_queue_bounded_manyproducer_manyconsumer_dequeue.c lfds710_queue_bounded_manyproducer_manyconsumer_enqueue.c lfds710_queue_bounded_manyproducer_manyconsumer_init.c lfds710_queue_bounded_manyproducer_manyconsumer_query.c \
              lfds710_queue_bounded_singleproducer_singleconsumer_cleanup.c lfds710_queue_bounded_singleproducer_singleconsumer_dequeue.c lfds710_queue_bounded_singleproducer_singleconsumer_enqueue.c lfds710_queue_bounded_singleproducer_singleconsumer_init.c lfds710_queue_bounded_singleproducer_singleconsumer_query.c \
              lfds710_queue_unbounded_manyproducer_manyconsumer_cleanup.c lfds710_queue_unbounded_manyproducer_manyconsumer_dequeue.c lfds710_queue_unbounded_manyproducer_manyconsumer_enqueue.c lfds710_queue_unbounded_manyproducer_manyconsumer_init.c lfds710_queue_unbounded_manyproducer_manyconsumer_query.c \
              lfds710_ringbuffer_cleanup.c lfds710_ringbuffer_init.c lfds710_ringbuffer_query.c lfds710_ringbuffer_read.c lfds710_ringbuffer_write.c \
              lfds710_stack_cleanup.c lfds710_stack_init.c lfds710_stack_pop.c lfds710_stack_push.c lfds710_stack_query.c
OBJECTS    := $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(SOURCES)))
SYSLIBS    := -lgcc

##### tools #####
DG                     := gcc
DGFLAGS_MANDATORY      := -MM
DGFLAGS_OPTIONAL       := -std=gnu89

CC                     := gcc
CFLAGS_MANDATORY       := -c -fno-strict-aliasing
CFLAGS_OPTIONAL        := -ffreestanding -nostdinc -std=gnu89 -Wall -Werror -Wno-unknown-pragmas -Wno-unused-but-set-variable -Wno-uninitialized
CFLAGS_MANDATORY_COV   := -O0 -ggdb -DCOVERAGE -fprofile-arcs -ftest-coverage
CFLAGS_MANDATORY_DBG   := -O0 -ggdb -D_DEBUG
CFLAGS_MANDATORY_PROF  := -O0 -ggdb -DPROF     -pg
CFLAGS_MANDATORY_REL   := -O2       -DNDEBUG
CFLAGS_MANDATORY_TSAN  := -O0 -ggdb -DTSAN     -fsanitize=thread -fPIC

AR                     := ar
ARFLAGS                :=
ARFLAGS_MANDATORY      := rcs
ARFLAGS_OPTIONAL       :=

LD                     := gcc
LDFLAGS_MANDATORY      := -shared -Wl,-soname,$(SONAME) -o $(SOPATHNAME)
LDFLAGS_OPTIONAL       := -nodefaultlibs -nostdlib -std=gnu89 -Wall -Werror
LDFLAGS_MANDATORY_COV  := -O0 -fprofile-arcs -ftest-coverage
LDFLAGS_MANDATORY_DBG  := -O0 -ggdb
LDFLAGS_MANDATORY_PROF := -O0 -pg
LDFLAGS_MANDATORY_REL  := -O2 -s
LDFLAGS_MANDATORY_TSAN := -O0 -fsanitize=thread -fPIC

##### build variants #####
ifeq ($(findstring so,$(MAKECMDGOALS)),so)
  CFLAGS_MANDATORY += -fPIC
endif

# TRD : default to debug
ifeq ($(MAKECMDGOALS),)
  CFLAGS_MANDATORY  += $(CFLAGS_MANDATORY_DBG)
  LDFLAGS_MANDATORY += $(LDFLAGS_MANDATORY_DBG)
endif

ifeq ($(findstring cov,$(MAKECMDGOALS)),cov)
  CFLAGS_MANDATORY  += $(CFLAGS_MANDATORY_COV)
  LDFLAGS_MANDATORY += $(LDFLAGS_MANDATORY_COV)
  SYSLIBS += -lgcov
endif

ifeq ($(findstring dbg,$(MAKECMDGOALS)),dbg)
  CFLAGS_MANDATORY  += $(CFLAGS_MANDATORY_DBG)
  LDFLAGS_MANDATORY += $(LDFLAGS_MANDATORY_DBG)
endif

ifeq ($(findstring prof,$(MAKECMDGOALS)),prof)
  CFLAGS_MANDATORY  += $(CFLAGS_MANDATORY_PROF)
  LDFLAGS_MANDATORY += $(LDFLAGS_MANDATORY_PROF)
endif

ifeq ($(findstring rel,$(MAKECMDGOALS)),rel)
  CFLAGS_MANDATORY  += $(CFLAGS_MANDATORY_REL)
  LDFLAGS_MANDATORY += $(LDFLAGS_MANDATORY_REL)
endif

ifeq ($(findstring tsan,$(MAKECMDGOALS)),tsan)
  CFLAGS_MANDATORY  += $(CFLAGS_MANDATORY_TSAN)
  LDFLAGS_MANDATORY += $(LDFLAGS_MANDATORY_TSAN)
endif

##### search paths #####
vpath %.c $(patsubst %,$(SRCDIR)/%:,$(SRCDIRS))

##### implicit rules #####
$(OBJDIR)/%.o : %.c
	$(DG) $(DGFLAGS_OPTIONAL) $(DGFLAGS) $(DGFLAGS_MANDATORY) $< >$(OBJDIR)/$*.d
	$(CC) $(CFLAGS_OPTIONAL) $(CFLAGS) $(CFLAGS_MANDATORY) -o $@ $<

##### explicit rules #####
$(ARPATHNAME) : $(OBJECTS)
	$(AR) $(ARFLAGS_OPTIONAL) $(ARFLAGS) $(ARFLAGS_MANDATORY) $(ARPATHNAME) $(OBJECTS)

$(SOPATHNAME) : $(OBJECTS)
	$(LD) $(LDFLAGS_OPTIONAL) $(LDFLAGS) $(LDFLAGS_MANDATORY) $(OBJECTS) -lgcov -lgcc -o $(SOPATHNAME)
	@ln -fs $(SOFILENAME) $(BINDIR)/$(SONAME)
	@ln -fs $(SOFILENAME) $(BINDIR)/$(SOBASENAME)

##### phony #####
.PHONY : clean ar_cov ar_dbg ar_prof ar_rel ar_tsan ar_vanilla ar_install ar_uninstall so_dbg so_prof so_rel so_tsan so_vanilla so_install so_uninstall

clean : 
	@rm -f $(BINDIR)/* $(OBJDIR)/*

ar_cov       : $(ARPATHNAME) # archive (.a), coverage
ar_dbg       : $(ARPATHNAME) # archive (.a), debug
ar_prof      : $(ARPATHNAME) # archive (.a), profiling
ar_rel       : $(ARPATHNAME) # archive (.a), release
ar_tsan      : $(ARPATHNAME) # archive (.a), thread sanitizer
ar_vanilla   : $(ARPATHNAME) # archive (.a), no specific-build arguments
ar_install   :
  # TRD : leading backslash to use command rather than alias
  #       as many Linux distros have a built-in alias to force
  #       a prompt ("y/n?") on file overwrite - silent and
  #       unexpected interference which breaks a makefile
	@mkdir -p $(DESTDIR)/$(INSLIBDIR)
	@\cp $(ARPATHNAME) $(DESTDIR)/$(INSLIBDIR)
	@mkdir -p $(DESTDIR)/$(INSINCDIR)
	@\cp -r $(INCDIR)/* $(DESTDIR)/$(INSINCDIR)
ar_uninstall :
	@rm    $(DESTDIR)/$(INSLIBDIR)/$(ARFILENAME)
	@rm -r $(DESTDIR)/$(INSINCDIR)/$(BINNAME)
	@rm -r $(DESTDIR)/$(INSINCDIR)/$(BINNAME).h

# TRD : so_cov currently disabled as it cannot work with -nostdlib -nodefaultlibs
# so_cov       : $(SOPATHNAME) # shared (.so), coverage
so_dbg       : $(SOPATHNAME) # shared (.so), debug
so_prof      : $(SOPATHNAME) # shared (.so), profiling
so_rel       : $(SOPATHNAME) # shared (.so), release
so_tsan      : $(SOPATHNAME) # shared (.so), thread sanitizer
so_vanilla   : $(SOPATHNAME) # shared (.so), no specific-build arguments
so_install   : 
	@mkdir -p $(DESTDIR)/$(INSINCDIR)
	@mkdir -p $(DESTDIR)/$(INSLIBDIR)
	@\cp $(SOPATHNAME) $(DESTDIR)/$(INSLIBDIR)
	@ldconfig -vn $(DESTDIR)/$(INSLIBDIR)
	@ln -s $(SONAME) $(DESTDIR)/$(INSLIBDIR)/$(SOBASENAME)
	@\cp -r $(INCDIR)/* $(DESTDIR)/$(INSINCDIR)
so_uninstall : 
	@rm -f $(DESTDIR)/$(INSLIBDIR)/$(SOFILENAME)
	@rm -f $(DESTDIR)/$(INSLIBDIR)/$(SOBASENAME)
	@rm -f $(DESTDIR)/$(INSLIBDIR)/$(SONAME)
	@rm -r $(DESTDIR)/$(INSINCDIR)/$(BINNAME)
	@rm -r $(DESTDIR)/$(INSINCDIR)/$(BINNAME).h

##### dependencies #####
-include $(DEPENDS)


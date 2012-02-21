VERSION := $(shell cat debian/version)
VARIANT := $(shell [ ! -z "$(DEB_HOST_ARCH)" ] && echo release || echo debug)

build:
	# build $(VARIANT) for $(DEB_HOST_ARCH)...
	@[ "$(DEB_BUILD_ARCH)" = "i386" ] && rm lib/ -rf ||:
	@bjam --user-config=build-config.jam build $(VARIANT) target-architecture=i386 2>/dev/null

distclean:
	# clean...
	@bjam --user-config=build-config.jam clean $(VARIANT) 2>/dev/null

install:
	# install $(VARIANT) for $(DEB_HOST_ARCH)...
	@bjam --user-config=build-config.jam install $(VARIANT) --prefix=${DESTDIR}/usr --bindir=${DESTDIR}/usr/games target-architecture=i386 2>/dev/null
	@install -d                                ${DESTDIR}/usr/lib/dwarf-fortress/
	@install -D libs/Dwarf_Fortress            ${DESTDIR}/usr/lib/dwarf-fortress/dwarf-fortress
	@ln -sf ../libdwarf-fortress-graphics.so.0 ${DESTDIR}/usr/lib/dwarf-fortress/libgraphics.so

	@[ ! "$(DEB_HOST_ARCH)" = "i386" ] && cp lib/libSDL_ttf-2.0.so.0   ${DESTDIR}/usr/lib/dwarf-fortress/ ||:
	@[ ! "$(DEB_HOST_ARCH)" = "i386" ] && cp lib/libSDL_image-1.2.so.0 ${DESTDIR}/usr/lib/dwarf-fortress/ ||:
	@[ ! "$(DEB_HOST_ARCH)" = "i386" ] && cp lib/libGLEW.so.1.6        ${DESTDIR}/usr/lib/dwarf-fortress/ ||:

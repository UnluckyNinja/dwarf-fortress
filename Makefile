UPSTREAM := $(VERSION)
DATE := $(shell date)
VARIANT := $(shell [ ! -z "$(DEB_HOST_ARCH)" ] && echo release || echo debug)

build: upstream-build mod-default-build mod-mayday-build mod-phoebus-build mod-ironhand-build i386-dependencies-build
	# build $(VARIANT) for $(DEB_HOST_ARCH)...
	@bjam --user-config=build-config.jam build $(VARIANT) target-architecture=i386 2>/dev/null

distclean: upstream-clean mod-default-clean mod-mayday-clean mod-phoebus-clean mod-ironhand-clean
	# clean...
	@bjam --user-config=build-config.jam clean $(VARIANT) 2>/dev/null
	@rm .pc -rf

install: upstream-install mod-default-install mod-mayday-install mod-phoebus-install mod-ironhand-install
	# install $(VARIANT) for $(DEB_HOST_ARCH)...
	@bjam --user-config=build-config.jam install $(VARIANT) --prefix=${DESTDIR}/usr --bindir=${DESTDIR}/usr/games target-architecture=i386 2>/dev/null
	@[ "$(DEB_HOST_ARCH)" = "amd64" ] && cp lib/libSDL_ttf-2.0.so.0   ${DESTDIR}/usr/lib/dwarf-fortress/ ||:
	@[ "$(DEB_HOST_ARCH)" = "amd64" ] && cp lib/libSDL_image-1.2.so.0 ${DESTDIR}/usr/lib/dwarf-fortress/ ||:
	@[ "$(DEB_HOST_ARCH)" = "amd64" ] && cp lib/libGLEW.so.1.6        ${DESTDIR}/usr/lib/dwarf-fortress/ ||:

upstream-clean:
	# clean upstream...
	@rm -rf data               2>/dev/null |:
	@rm -rf libs               2>/dev/null |:
	@rm -rf lib                2>/dev/null |:

upstream-build:
	# build upstream...
	@git checkout -q upstream/${VERSION} data/ libs/Dwarf_Fortress
	@git reset    -q
	@find -iname 'thumbs.db' -delete
	@find data/ -type f -exec dos2unix -q '{}' \;

upstream-install:
	# install upstream...
	@install -d                                ${DESTDIR}/usr/lib/dwarf-fortress/
	@install -D libs/Dwarf_Fortress            ${DESTDIR}/usr/lib/dwarf-fortress/dwarf-fortress
	@ln -sf ../libdwarf-fortress-graphics.so.0 ${DESTDIR}/usr/lib/dwarf-fortress/libgraphics.so

	@install -d    ${DESTDIR}/usr/share/games/dwarf-fortress/data/
	@cp -rf data/* ${DESTDIR}/usr/share/games/dwarf-fortress/data/

i386-dependencies-build:
	# checking out i386-dependencies...
	@[ "$(DEB_BUILD_ARCH)" = "amd64" ] && git checkout origin/i386-dependencies -- lib/ || mkdir lib/
	@[ "$(DEB_BUILD_ARCH)" = "amd64" ] && git reset    -q                               ||:


# ------------------------------------------------------------------------------
# Mod Phoebus
# ------------------------------------------------------------------------------

checkout-mod-phoebus:
	@git clean -fdx
	@git checkout   -q mod/phoebus/upstream


mod-phoebus-import-checks:
	@if [ -z "${UPSTREAM}" ]; then echo "Please set VERSION and UPSTREAM" && return 1; fi
	@if [ ! -z "`git status --short`" ]; then echo "Please stash changes" && return 1; fi


mod-phoebus-import-unzip:
	# checkout mod phoebus from archive...
	@git rm -q -r mod/ |:
	@7z x -ir\!data/ -ir\!raw/ ../Phoebus_${UPSTREAM}.zip

	# copy mod phoebus...
	@cp data/init/colors.txt                 colors.txt
	@cp data/art/Phoebus_16x16.png           tiles.png
	@cp data/art/Phoebus_16x16_TextClean.png tiles-lite.png
	@cp -rf raw/* ./
	@find -iname 'thumbs.db' -delete

	@rm -rf data
	@rm -rf raw

mod-phoebus-import-commit:
	@git add .
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git commit -q -am "Imported upstream/${VERSION} mod-phoebus"
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git tag       -am "Phoebus mod upstream version ${VERSION}." mod/phoebus/upstream/${VERSION}

mod-phoebus-import: mod-phoebus-import-checks checkout-mod-phoebus mod-phoebus-import-unzip file-cleanup mod-phoebus-import-commit checkout-back


mod-phoebus-merge-commit:
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git merge mod/default/upstream/${VERSION} 
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git tag       -am "Phoebus mod upstream version ${VERSION}." mod/phoebus/upstream/${VERSION}

mod-phoebus-merge: mod-phoebus-import-checks checkout-mod-phoebus mod-phoebus-merge-commit checkout-back


debian-merge-mod-phoebus-upstream:
	# merging mod/phoebus/upstream version ${VERSION} into mod/phoebus/debian
	@git merge mod/phoebus/upstream/${VERSION} --no-commit
	@sed -i -re "s@`cat debian/version`@${VERSION}@" `find debian/ -type f | grep -v changelog`
	@dch -D unstable -v ${VERSION}-1 "New upstream version"
	@git add debian/

debian-merge-mod-phoebus-commit:
	@git commit -q -am "`cat .git/MERGE_MSG`"

debian-merge-mod-phoebus: debian-merge-checks checkout-mod-phoebus-debian debian-merge-mod-phoebus-upstream debian-merge-mod-phoebus-commit checkout-back


# ------------------------------------------------------------------------------
# Mod Ironhand
# ------------------------------------------------------------------------------

checkout-mod-ironhand:
	@git clean -fdx
	@git checkout   -q mod/ironhand/upstream


mod-ironhand-import-checks:
	@if [ -z "${UPSTREAM}" ]; then echo "Please set VERSION and UPSTREAM" && return 1; fi
	@if [ ! -z "`git status --short`" ]; then echo "Please stash changes" && return 1; fi


mod-ironhand-import-unzip:
	# checkout mod ironhand from archive...
	@git rm -q -r mod/ |:
	@7z x -ir\!DF/data/ -ir\!DF/raw/ '../Ironhand ${UPSTREAM}.zip'
	@mv DF/* ./
	@rm DF -rf

	# copy mod ironhand...
	@cp data/init/colors.txt            colors.txt
	@cp data/art/ironhand.png           tiles.png
	@cp data/art/ironhand.png           tiles-lite.png
	@cp -rf raw/* ./
	@find -iname 'thumbs.db' -delete

	@rm -rf data
	@rm -rf raw

mod-ironhand-import-commit:
	@git add .
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git commit -q -am "Imported upstream/${VERSION} mod-ironhand"
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git tag       -am "Ironhand mod upstream version ${VERSION}." mod/ironhand/upstream/${VERSION}

mod-ironhand-import: mod-ironhand-import-checks checkout-mod-ironhand mod-ironhand-import-unzip file-cleanup mod-ironhand-import-commit checkout-back


mod-ironhand-merge-commit:
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git merge mod/default/upstream/${VERSION} 
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git tag       -am "Ironhand mod upstream version ${VERSION}." mod/ironhand/upstream/${VERSION}

mod-ironhand-merge: mod-ironhand-import-checks checkout-mod-ironhand mod-ironhand-merge-commit checkout-back


debian-merge-mod-ironhand-upstream:
	# merging mod/ironhand/upstream version ${VERSION} into mod/ironhand/debian
	@git merge mod/ironhand/upstream/${VERSION} --no-commit
	@sed -i -re "s@`cat debian/version`@${VERSION}@" `find debian/ -type f | grep -v changelog`
	@dch -D unstable -v ${VERSION}-1 "New upstream version"
	@git add debian/

debian-merge-mod-ironhand-commit:
	@git commit -q -am "`cat .git/MERGE_MSG`"

debian-merge-mod-ironhand: debian-merge-checks checkout-mod-ironhand-debian debian-merge-mod-ironhand-upstream debian-merge-mod-ironhand-commit checkout-back


# ------------------------------------------------------------------------------
# Mod Mayday
# ------------------------------------------------------------------------------

checkout-mod-mayday:
	@git clean -fdx
	@git checkout   -q mod/mayday/upstream


mod-mayday-import-checks:
	@if [ -z "${UPSTREAM}" ]; then echo "Please set VERSION and UPSTREAM" && return 1; fi
	@if [ ! -z "`git status --short`" ]; then echo "Please stash changes" && return 1; fi


mod-mayday-import-unzip:
	# checkout mod mayday from archive...
	@git rm -q -r * |:
	@7z x -ir\!data/ -ir\!raw/ ../dfg_${UPSTREAM}_win.zip

	# copy mod mayday...
	@cp data/init/colors.txt             colors.txt
	@cp data/art/mayday.png              tiles.png
	@cp data/art/mayday.png              tiles-lite.png
	@cp -rf raw/* ./
	@find -iname 'thumbs.db' -delete

	@rm -rf data
	@rm -rf raw

mod-mayday-import-commit:
	@git add .
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git commit -q -am "Imported upstream/${VERSION} mod-mayday"
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git tag       -am "Mayday mod upstream version ${VERSION}." mod/mayday/upstream/${VERSION}

mod-mayday-import: mod-mayday-import-checks checkout-mod-mayday mod-mayday-import-unzip file-cleanup mod-mayday-import-commit checkout-back


mod-mayday-merge-commit:
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git merge mod/default/upstream/${VERSION} 
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git tag       -am "Mayday mod upstream version ${VERSION}." mod/mayday/upstream/${VERSION}

mod-mayday-merge: mod-mayday-import-checks checkout-mod-mayday mod-mayday-merge-commit checkout-back


debian-merge-mod-mayday-upstream:
	# merging mod/mayday/upstream version ${VERSION} into mod/mayday/debian
	@git merge mod/mayday/upstream/${VERSION} --no-commit
	@sed -i -re "s@`cat debian/version`@${VERSION}@" `find debian/ -type f | grep -v changelog`
	@dch -D unstable -v ${VERSION}-1 "New upstream version"
	@git add debian/

debian-merge-mod-mayday-commit:
	@git commit -q -am "`cat .git/MERGE_MSG`"

debian-merge-mod-mayday: debian-merge-checks checkout-mod-mayday-debian debian-merge-mod-mayday-upstream debian-merge-mod-mayday-commit checkout-back


# ------------------------------------------------------------------------------
# Mod Default
# ------------------------------------------------------------------------------

checkout-mod-default:
	@git clean -fdx
	@git checkout   -q mod/default/upstream


mod-default-import-checks:
	@if [ -z "${VERSION}" ]; then echo "Please set VERSION" && return 1; fi
	@if [ ! -z "`git status --short`" ]; then echo "Please stash changes" && return 1; fi

mod-default-import-upstream:
	# checkout mod default from upstream...
	@git merge upstream/${VERSION} --strategy=subtree --no-ff --no-commit

mod-default-import-commit:
	@GIT_AUTHOR_DATE="`git log -1 --pretty=%ai upstream/${VERSION}`" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git commit -q  -m "Imported upstream/${VERSION} mod-default"
	@GIT_AUTHOR_DATE="`git log -1 --pretty=%ai upstream/${VERSION}`" GIT_COMMITTER_DATE="$${GIT_AUTHOR_DATE}" git tag       -am "Default mod upstream version ${VERSION}." mod/default/upstream/${VERSION}

mod-default-import: mod-default-import-checks checkout-mod-default mod-default-import-upstream file-cleanup mod-default-import-commit checkout-back mod-mayday-merge mod-ironhand-merge mod-phoebus-merge


mod-default-clean:
	# clean mod default...
	@rm -rf mods/default/      2>/dev/null |:

mod-default-build: mod-default-clean
	# build mod default...
	@git checkout -q mod/default/${VERSION} mod/
	@git reset    -q
	@install -d mods/
	@mv mod/ mods/default/

mod-default-install:
	# install mod default...
	@install -d ${DESTDIR}/usr/share/games/dwarf-fortress/mods/
	@cp -rf mods/default/ ${DESTDIR}/usr/share/games/dwarf-fortress/mods/


debian-merge-mod-default-upstream:
	# merging mod/default/upstream version ${VERSION} into mod/default/debian
	@git merge mod/default/upstream/${VERSION} --no-commit
	@sed -i -re "s@`cat debian/version`@${VERSION}@" `find debian/ -type f | grep -v changelog`
	@dch -D unstable -v ${VERSION}-1 "New upstream version"
	@git add debian/

debian-merge-mod-default-commit:
	@git commit -q -am "`cat .git/MERGE_MSG`"

debian-merge-mod-default: debian-merge-checks checkout-mod-default-debian debian-merge-mod-default-upstream debian-merge-mod-default-commit checkout-back


# ------------------------------------------------------------------------------
# Upstream
# ------------------------------------------------------------------------------

checkout-upstream:
	@git clean -fdx
	@git checkout   -q upstream


upstream-import-checks:
	@if [ -z "${UPSTREAM}" ] || [ -z "${VERSION}" ]; then echo "Please set VERSION and UPSTREAM" && return 1; fi
	@if [ ! -z "`git status --short`" ]; then echo "Please stash changes" && return 1; fi


upstream-import-untar:
	# importing upstream version ${VERSION}
	@git rm -q -r *
	@tar xjf ../df_${UPSTREAM}_linux.tar.bz2 --strip-components 1

upstream-import-commit:
	@git add .
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$(DATE)" git commit -q -am "Imported df_${UPSTREAM}_linux.tar.bz2"
	@GIT_AUTHOR_DATE="$(DATE)" GIT_COMMITTER_DATE="$(DATE)" git tag       -am "Upstream version ${VERSION}." upstream/${VERSION}

upstream-import: upstream-import-checks checkout-upstream upstream-import-untar file-cleanup upstream-import-commit checkout-back


# ------------------------------------------------------------------------------
# Debian
# ------------------------------------------------------------------------------

checkout-debian:
	@git clean -fdx
	@git checkout   -q debian

checkout-mod-default-debian:
	@git clean -fdx
	@git checkout   -q mod/default/debian

checkout-mod-mayday-debian:
	@git clean -fdx
	@git checkout   -q mod/mayday/debian

checkout-mod-phoebus-debian:
	@git clean -fdx
	@git checkout   -q mod/phoebus/debian

checkout-mod-ironhand-debian:
	@git clean -fdx
	@git checkout   -q mod/ironhand/debian


build-debian:
	@gbp-pq export
	@git-buildpackage --git-ignore-new --git-tag

build-mod-default-debian:
	@git-buildpackage --git-tag

build-mod-mayday-debian:
	@git-buildpackage --git-tag

build-mod-phoebus-debian:
	@git-buildpackage --git-tag

build-mod-ironhand-debian:
	@git-buildpackage --git-tag

debian-package: checkout-debian build-debian checkout-back checkout-mod-default-debian build-mod-default-debian checkout-back checkout-mod-mayday-debian build-mod-mayday-debian checkout-back checkout-mod-phoebus-debian build-mod-phoebus-debian checkout-back checkout-mod-ironhand-debian build-mod-ironhand-debian checkout-back

debian-merge-checks:
	@if [ -z "${VERSION}" ]; then echo "Please set VERSION" && return 1; fi
	@if [ ! -z "`git status --short`" ]; then echo "Please stash changes" && return 1; fi

debian-merge-upstream:
	# merging upstream version ${VERSION} into debian
	@git merge upstream/${VERSION} --no-commit
	@sed -i -re "s@`cat debian/version`@${VERSION}@" `find debian/ -type f | grep -v changelog`
	@dch -D unstable -v ${VERSION}-1 "New upstream version"
	@git add debian/

debian-merge-commit:
	@git commit -q -am "`cat .git/MERGE_MSG`"

debian-merge: debian-merge-checks checkout-debian debian-merge-upstream debian-merge-commit checkout-back


# ------------------------------------------------------------------------------
# Common
# ------------------------------------------------------------------------------

file-cleanup:
	# clean file permissions...
	@find -type f -exec chmod 644 '{}' \;
	@chmod +x df libs/* --quiet |:

	# convert to unix...
	@find -type f -exec dos2unix -q '{}' \;

	# clean white spaces...
	@find -type f -iname '*.txt' -exec sed -i '{}' -re 's/[\t ]+$$//'                  \;
	@find -type f -iname '*.txt' -exec sed -i '{}'  -e ':a; /^\n*$$/{$$d;N;};/\n$$/ba' \;
	@find -type f -iname '*.txt' -exec sed -i '{}' -re '$$G'                           \;


checkout-back:
	@git checkout  -q -

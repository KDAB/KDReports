*-g++* {
  NORMAL_CFLAGS = -Wno-long-long -ansi
  NORMAL_CXXFLAGS = \
	-Wnon-virtual-dtor -Wundef -Wcast-align \
	-Wchar-subscripts -Wpointer-arith \
	-Wwrite-strings -Wformat-security \
        -Wmissing-format-attribute -Woverloaded-virtual

  # -Wconversion gives too many warnings from Qt-4.4.3 with gcc-4.3.2 (was fine with gcc-4.2.4), so removing it
  # -Wpacked gives warnings with Qt 4.8, removed

  NORMAL_CFLAGS += -pedantic

  CONFIG(debug, debug|release) {
    NORMAL_CXXFLAGS += -O0 -g3
    NORMAL_CXXFLAGS -= -g
  }
  USABLE_CXXFLAGS = -Wold-style-cast
  HARD_CXXFLAGS = -Weffc++ -Wshadow
  PITA_CXXFLAGS = -Wunreachable-code

  QMAKE_CFLAGS   += $$NORMAL_CFLAGS
  QMAKE_CXXFLAGS += $$NORMAL_CFLAGS $$NORMAL_CXXFLAGS

  # Mirko: removed that during development in trunk, should be
  # reenabled once going towards a release: -Werror 
  QMAKE_CFLAGS_WARN_ON   += $$NORMAL_CFLAGS
  # Mirko: removed that during development in trunk, should be
  # reenabled once going towards a release: -Werror 
  QMAKE_CXXFLAGS_WARN_ON += $$NORMAL_CFLAGS $$NORMAL_CXXFLAGS

  #QMAKE_CXXFLAGS_WARN_ON += $$USABLE_CXXFLAGS
  #QMAKE_CXXFLAGS_WARN_ON += $$HARD_CXXFLAGS # headers must compile with this, code doesn't need to; needs patched Qt
  #QMAKE_CXXFLAGS_WARN_ON += $$PITA_CXXFLAGS # header would be nice, but it's probably pointless, due to noise from Qt and libstdc++

}

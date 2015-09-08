# Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Flora License, Version 1.1 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://floralicense.org/license/
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @author   Leerang Song (leerang.song@samsung.com)

SET(POFILES
hy.po az.po eu.po bg.po ca.po cs.po da.po nl_NL.po en.po en_US.po
et.po fi.po fr_FR.po gl.po hr.po ka.po de_DE.po el_GR.po hu.po is.po
it_IT.po kk.po ko_KR.po lv.po lt.po nb.po pl.po pt_PT.po pt_BR.po ro.po
ru_RU.po sr.po sk.po sl.po es_ES.po es_MX.po sv.po tr_TR.po uk.po uz.po
fr_CA.po ja_JP.po es_US.po)

ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALE_DIR}\"")
SET(MSGFMT "/usr/bin/msgfmt")

    FOREACH(pofile ${POFILES})
    SET(pofile ${CMAKE_CURRENT_SOURCE_DIR}/${pofile})
    MESSAGE("PO: ${pofile}")
    GET_FILENAME_COMPONENT(absPofile ${pofile} ABSOLUTE)
    GET_FILENAME_COMPONENT(lang ${absPofile} NAME_WE)

    SET(moFile ${CMAKE_CURRENT_BINARY_DIR}/${lang}.mo)
    ADD_CUSTOM_COMMAND( OUTPUT ${moFile}
        COMMAND ${MSGFMT} -o ${moFile} ${absPofile}
        DEPENDS ${absPofile} )
    INSTALL(FILES ${moFile}
        DESTINATION ${LOCALE_DIR}/${lang}/LC_MESSAGES
        RENAME  ${PROJECT_NAME}.mo)
SET(moFiles ${moFiles} ${moFile})
ENDFOREACH(pofile)

MESSAGE(".mo files: ${moFiles}")
ADD_CUSTOM_TARGET(po ALL DEPENDS ${moFiles})


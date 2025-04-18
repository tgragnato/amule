CMAKE_MINIMUM_REQUIRED (VERSION 4.0)
MESSAGE (STATUS "Generating ${HEADER_FILE}")
FILE (READ License.abstract LICENSE_TEXT)
STRING (REGEX REPLACE "\n\n+" "" LICENSE_TEXT ${LICENSE_TEXT})
STRING (REGEX REPLACE "\n" "\n// " LICENSE_TEXT ${LICENSE_TEXT})

GET_FILENAME_COMPONENT (ABSTRACT ${HEADER_FILE} NAME_WE)
STRING (TOUPPER "${ABSTRACT}" DEFINE_NAME)
SET (ABSTRACT "${ABSTRACT}.abstract")
FILE (WRITE ${HEADER_FILE} "${LICENSE_TEXT}\n")

FILE (STRINGS ${ABSTRACT} CONTENTS NEWLINE_CONSUME)
STRING (REGEX REPLACE "\n" ";" CONTENTS ${CONTENTS})

FOREACH (LINE ${CONTENTS})
	IF ("${LINE}" MATCHES "^DataType")
		UNSET (LINE)
	ENDIF ("${LINE}" MATCHES "^DataType")

	IF ("${LINE}" MATCHES "^FileContent")
		FILE (APPEND ${HEADER_FILE} "\n// Purpose:\n")
		STRING (REGEX REPLACE "FileContent *" "// " LINE ${LINE})
		FILE (APPEND ${HEADER_FILE} "${LINE}\n\n")
		GET_FILENAME_COMPONENT (FILE_NAME ${HEADER_FILE} NAME_WE)
		STRING (TOUPPER "${FILE_NAME}" DEFINE_NAME)
		FILE (APPEND ${HEADER_FILE} "#ifndef __${DEFINE_NAME}_H__\n")
		FILE (APPEND ${HEADER_FILE} "#define __${DEFINE_NAME}_H__\n\n")
	ENDIF ("${LINE}" MATCHES "^FileContent")

	IF (NOT "${LINE}" STREQUAL "" AND NOT "${LINE}" MATCHES "^#")
		IF (${IN_BLOCK})
			IF ("${LINE}" STREQUAL "[/Section]")
				UNSET (LINE)
				IF (${IN_CODE_BLOCK})
					SET (IN_CODE_BLOCK FALSE)
					FILE (APPEND ${HEADER_FILE} "\t${PREVIOUS_LINE}\n")
					FILE (APPEND ${HEADER_FILE} "};\n\n")
					UNSET (PREVIOUS_LINE)
				ENDIF (${IN_CODE_BLOCK})

				IF (${IN_TYPEDEF})
					SET (IN_TYPEDEF FALSE)
					FILE (APPEND ${HEADER_FILE} "\n")
				ENDIF (${IN_TYPEDEF})
			ENDIF ("${LINE}" STREQUAL "[/Section]")

			IF (${IN_CODE_BLOCK})
				IF (NOT ${PREVIOUS_LINE} STREQUAL "")
					FILE (APPEND ${HEADER_FILE} "\t${PREVIOUS_LINE},\n")
				ENDIF (NOT ${PREVIOUS_LINE} STREQUAL "")
				STRING (REGEX REPLACE "^\t+" "" LINE "${LINE}")
				STRING (REGEX REPLACE "[ |\t]+" " = " PREVIOUS_LINE "${LINE}")
			ENDIF (${IN_CODE_BLOCK})

			IF (${IN_TYPEDEF})
					STRING (REGEX REPLACE "[ |\t]" ";" LINE ${LINE})
					LIST (GET LINE 0 TYPE_DEF_NAME)
					LIST (GET LINE -1 TYPE_DEF_TYPE)
					FILE (APPEND ${HEADER_FILE} "typedef ${TYPE_DEF_TYPE}_t ${TYPE_DEF_NAME};\n")
					UNSET (TYPE_DEF_NAME)
					UNSET (TYPE_DEF_TYPE)
			ENDIF (${IN_TYPEDEF})

			IF ("${LINE}" MATCHES "^Type ")
				STRING (REGEX REPLACE "^Type " "" TYP ${LINE})
				IF (${TYP} STREQUAL "TypeDef")
					SET (IN_TYPEDEF TRUE)
				ENDIF (${TYP} STREQUAL "TypeDef")
			ENDIF ("${LINE}" MATCHES "^Type ")

			IF ("${LINE}" MATCHES "^Name ")
				STRING (REGEX REPLACE "^Name " "" NAME ${LINE})

				IF (NOT ${TYP} STREQUAL "")
					STRING (TOLOWER "${TYP}" TYP)
					FILE (APPEND ${HEADER_FILE} "${TYP} ${NAME} {\n")
					UNSET (TYPE)
					UNSET (NAME)
					SET (IN_CODE_BLOCK TRUE)
					SET (FIRST_LOOP TRUE)
				ELSE (NOT ${TYP} STREQUAL "")
					MESSAGE ("Name without Type in ${ABSTRACT}")
				ENDIF (NOT ${TYP} STREQUAL "")
			ENDIF ("${LINE}" MATCHES "^Name ")
		ELSE (${IN_BLOCK})
			IF ("${LINE}" STREQUAL "[Section Content]")
				SET (IN_BLOCK TRUE)
			ENDIF ("${LINE}" STREQUAL "[Section Content]")
		ENDIF (${IN_BLOCK})
	ENDIF (NOT "${LINE}" STREQUAL "" AND NOT "${LINE}" MATCHES "^#")
ENDFOREACH (LINE ${CONTENTS})

FILE (APPEND ${HEADER_FILE} "#ifdef DEBUG_EC_IMPLEMENTATION\n\n")

FOREACH (LINE ${CONTENTS})
	IF (NOT "${LINE}" MATCHES "^#" OR NOT "${LINE}" MATCHES "")
		IF (${FOUND_ENUM})
			IF (NOT "${LINE}" STREQUAL "[/Section]" AND NOT "${LINE}" MATCHES "^Name " AND NOT "${LINE}" MATCHES "^DataType ")
				STRING (REGEX REPLACE "^[\t| ]" "" LINE ${LINE})
				STRING (REGEX REPLACE "[\t| ]" ";" LINE ${LINE})
				LIST (GET LINE 0 TAG_NAME)
				LIST (GET LINE -1 TAG_CODE)
				FILE (APPEND ${HEADER_FILE} "\t\tcase ${TAG_CODE}: return wxT(\"${TAG_NAME}\");\n")
			ENDIF (NOT "${LINE}" STREQUAL "[/Section]" AND NOT "${LINE}" MATCHES "^Name " AND NOT "${LINE}" MATCHES "^DataType ")

			IF ("${LINE}" STREQUAL "[/Section]")
				FILE (APPEND ${HEADER_FILE} "\t\tdefault: return CFormat(wxT(\"unknown %d 0x%x\")) % arg % arg;\n\t}\n}\n\n")
				SET (FOUND_ENUM FALSE)
			ENDIF ("${LINE}" STREQUAL "[/Section]")

			IF ("${LINE}" MATCHES "^Name ")
				STRING (REGEX REPLACE "^Name " "" NAME ${LINE})
			ENDIF ("${LINE}" MATCHES "^Name ")

			IF ("${LINE}" MATCHES "^DataType ")
				STRING (REGEX REPLACE "^DataType " "" TYP ${LINE})
				FILE (APPEND ${HEADER_FILE} "wxString GetDebugName${NAME}(${TYP} arg)\n{\n\tswitch (arg) {\n")
			ENDIF ("${LINE}" MATCHES "^DataType ")
		ENDIF (${FOUND_ENUM})

		IF ("${LINE}" MATCHES "^Type ")
			STRING (REGEX REPLACE "^Type " "" LINE ${LINE})
			IF ("${LINE}" STREQUAL "Enum")
				SET (FOUND_ENUM TRUE)
			ENDIF ("${LINE}" STREQUAL "Enum")
		ENDIF ("${LINE}" MATCHES "^Type ")
	ENDIF (NOT "${LINE}" MATCHES "^#" OR NOT "${LINE}" MATCHES "")
ENDFOREACH (LINE ${CONTENTS})

FILE (APPEND ${HEADER_FILE} "#endif	// DEBUG_EC_IMPLEMENTATION\n\n")

FILE (APPEND ${HEADER_FILE} "#endif // __${DEFINE_NAME}_H__\n")

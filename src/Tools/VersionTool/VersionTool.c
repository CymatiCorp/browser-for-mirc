/* mChef - VersionTool.c
 * Copyright (C) 2009 Genscripts
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void WriteChefHeader(FILE *fp, int versionMajor, int versionMinor, int versionPatch, int versionBuild)
{
    /* Possible error? Better write a new header then. */
    if (versionMajor == 0)
        versionMajor++;
    
    printf("  VERSION_MAJOR: %i\n  VERSION_MINOR: %i\n  VERSION_PATCH: %i\n  VERSION_BUILD: %i\n", versionMajor, versionMinor, versionPatch, versionBuild);

    fprintf(fp,
        "/* mChef - Chef.h\n"
        " * Copyright (C) 2009 Genscripts\n"
        " *\n"
        " * This program is free software: you can redistribute it and/or modify\n"
        " * it under the terms of the GNU General Public License as published by\n"
        " * the Free Software Foundation, either version 3 of the License, or\n"
        " * (at your option) any later version.\n"
        " *\n"
        " * This program is distributed in the hope that it will be useful,\n"
        " * but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        " * GNU General Public License for more details.\n"
        " *\n" 
        " * You should have received a copy of the GNU General Public License\n"
        " * along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
        " *\n"
        " */\n"
        "#ifndef __CHEF_H__\n"
        "#   define __CHEF_H__\n\n"
        "/* Change only these macros */\n"
        "#define VERSION_MAJOR           %i\n"
        "#define VERSION_MINOR           %i\n"
        "#define VERSION_PATCH           %i\n\n\n"
        "/* Changing any of the macros below will only be waisting your time.*/\n\n"
        "/* Number of times that the project has been built */\n"
        "#define VERSION_BUILD           %i\n\n"
        "/* Current mChef Version */\n"
        "#define MCHEF_VERSION_STR        \"%i.%i.%i\"\n"
        "#define MCHEF_VERSION_STRWC     L\"%i.%i.%i\"\n\n"
        "#define VERSION_FULL            \"%i.%i.%i\"\n"
        "#define VERSION_WFULL           L\"%i.%i.%i\"\n\n"
        "#define VERSION_FILE            %i,%i,%i,%i\n"
        "#define VERSION_PRODUCT         %i,%i,%i,%i\n"
        "#define VERSION_FSTR            \"%i, %i, %i, %i\"\n"
        "#define VERSION_PSTR            \"%i, %i, %i, %i\"\n\n"
        "#endif\n\n"
        "/* EOF */\n\n\n",
        versionMajor, versionMinor, versionPatch, versionBuild,
        versionMajor, versionMinor, versionBuild,
        versionMajor, versionMinor, versionBuild,
        versionMajor, versionMinor, versionBuild,
        versionMajor, versionMinor, versionBuild,
        versionMajor, versionMinor, versionPatch, versionBuild,
        versionMajor, versionMinor, versionPatch, versionBuild,
        versionMajor, versionMinor, versionPatch, versionBuild,
        versionMajor, versionMinor, versionPatch, versionBuild);
}
int main(int parc, char *parv[])
{
    FILE *fp = NULL;
    char lineBuffer[BUFSIZ];
    char *token = NULL, *delims = " \t";
    int lineNum, versionMajor, versionMinor, versionPatch, versionBuild, curBuild;
    int i = 0;

    lineNum = versionMajor = versionMinor = versionPatch = versionBuild = curBuild = 0;

    if (parc != 2) {
        printf("Syntax: VersionTool.exe \"../include/ChefVersion.h\"\n");

        return 0;
    }
    printf("mChef VersionTool\n");
    printf("  Reading: %s\n", parv[1]);

    if (!(fp = fopen(parv[1], "r"))) {
        printf("Error opening \"%s\", does the file exist?\n", parv[1]);
        return 0;
    }
    memset(lineBuffer, 0, BUFSIZ);

    while (fgets(lineBuffer, sizeof(lineBuffer), fp)) {
        token = strtok(lineBuffer, delims);
        if (token && !strncmp(token, "#define", 6)) {
            token = strtok(NULL, delims);
            if (!token)
                continue;
            if (!strcmp(token, "VERSION_MAJOR")) {
                token = strtok(NULL, delims);
                versionMajor = atoi(token);
            } else if (!strcmp(token, "VERSION_MINOR")) {
                token = strtok(NULL, delims);
                versionMinor = atoi(token);
            } else if (!strcmp(token, "VERSION_PATCH")) {
                token = strtok(NULL, delims);
                versionPatch = atoi(token);
            } else if (!strcmp(token, "VERSION_BUILD")) {
                token = strtok(NULL, delims);
                versionBuild = atoi(token);
            }
        }
        lineNum++;
    }
    fclose(fp);

    if (!(fp = fopen("../.svn/entries", "r"))) {
        printf("Error opening svn entries file.\n");
        return 0;
    }
    memset(lineBuffer, 0, BUFSIZ);
    for (i = 0; i < 4 && fgets(lineBuffer, sizeof(lineBuffer), fp); i++)
        ;;
    curBuild = atoi(lineBuffer);

    fclose(fp);

    if (curBuild != versionBuild) {
        printf("Writing new %s\n", parv[1]);
        if (!(fp = fopen(parv[1], "w"))) {
            printf("Error opening file for witting\n");
            return 0;
        }
        WriteChefHeader(fp, versionMajor, versionMinor, versionPatch, curBuild);
        fclose(fp);
    }
    printf("  mChef VersionTool Done\n");

    return 0;
}

/* EOF */


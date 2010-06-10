/* AUTHOR:				Naveen Kumar Molleti
 * EMAIL:				nerd.naveen@gmail.com
 * GIT Read-only URL:	git://github.com/NOLFXceptMe/pcmanfm2k10.git
 *
 * Test code for using GKeyFile to read .desktop files to Action definitions [ref. http://www.nautilus-actions.org/?q=node/377], and to work out a possible data structure
 */

#include "parser.h"

int main()
{
	int ret_val = parse("open-terminal.desktop");
	return 0;
}

#include <string>
#include <vector>
#include <ncurses.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

int width;
int height;
using cpp_int = boost::multiprecision::cpp_int;
cpp_int cookie = cpp_int("0x8000000000000");

void printCookie(WINDOW * window);
void printItem(WINDOW * window);

struct Item
{
	std::string name;
	cpp_int const baseCost;
	cpp_int const baseCps;
	cpp_int cost;
	cpp_int cps;
	cpp_int number;

	Item(std::string && name, cpp_int && baseCost, cpp_int && baseCps)
	:
	name(name),
	baseCost(baseCost),
	baseCps(baseCps),
	cost(baseCost),
	cps(baseCps),
	number(0)
	{
	}

	void buy()
	{
		if (cookie >= cost)
		{
			++number;
			cookie -= cost;
			//cost = baseCost * std::pow(1.15, (double)number);
			cost = baseCost * (int)std::pow(1.15, (int)number);
		}
	}

};

std::vector<Item> item
{
	{"++", cpp_int("0x10"), cpp_int("0x1")},
	{"$EDITOR", cpp_int("0x20"), cpp_int("0x4")},
	{"Farm", cpp_int("0x400"), cpp_int("0x10")},
	{"Mine", cpp_int("0x4000"), cpp_int("0x40")},
	{"Factory", cpp_int("0x5000"), cpp_int("0x200")},
	{"Bank", cpp_int("0x200000"), cpp_int("0xA00")},
	{"Temple", cpp_int("0x300000"), cpp_int("0x4000")},
	{"Wizard Tower", cpp_int("0x20000000"), cpp_int("0x10000")},
	{"Shipment", cpp_int("0x70000000"), cpp_int("0x80000")},
	{"Alchemy Lab", cpp_int("0x1000000000"), cpp_int("0x300000")},
	{"Portal", cpp_int("0x10000000000"), cpp_int("0x2000000")},
	{"Time Machine", cpp_int("0xE0000000000"), cpp_int("0x9000000")},
	{"Antimatter Condenser", cpp_int("0xB00000000000"), cpp_int("0x40000000")},
	{"Prism", cpp_int("0x8000000000000"), cpp_int("0x200000000")}
};

int main()
{
	initscr();
	clear();
	noecho();
	nonl();
	cbreak();
	curs_set(0);
	//halfdelay(1);
	timeout(1);
	nodelay(stdscr, true);
	keypad(stdscr, true);
	//mousemask(BUTTON1_RELEASED, NULL);
	mousemask(BUTTON1_CLICKED, NULL);
	getmaxyx(stdscr, height, width);
	notimeout(stdscr, false);


	/*attron(A_REVERSE);
	mvprintw(23, 1, "Click on Exit to quit (Works best in a virtual console)");
	attroff(A_REVERSE);
	refresh();
	*/

	WINDOW * windowCookie;
	windowCookie = newwin(height, width / 3, 0, 0);
	nodelay(windowCookie, true);

	WINDOW * windowItem;
	windowItem = newwin(height, width / 3, 0, ((width * 2) / 3));
	nodelay(windowItem, true);

	printCookie(windowCookie);
	doupdate();

	MEVENT event;
	while (true)
	{
		//int c = wgetch(windowCookie);
		int c = getch();
		static int jjj = 0;
		//mvwprintw(windowCookie, 2, 10, "%0X %0X", c, jjj++);
		switch (c)
		{
			case KEY_MOUSE:
			if (getmouse(&event) == OK)
			{
				if (event.bstate & BUTTON1_RELEASED)
				{
					++cookie;
					int a = ((event.y - 2) / 4);
					if ((0 <= a && a < item.size()) && (width * 2 / 3) <= event.x)
					{
						item[a].buy();
					}
				}
				//mvprintw(30, 0, "%d, %d", event.x, event.y);
			}
			break;

			case KEY_ENTER:
				++cookie;
			break;

			case 'a':
				++cookie;
			break;

			default:
			break;
		}
		{
			static int time = 0;
			if (time >= 1000)
			{
				for (auto & e : item)
				{
					cookie += e.number * e.cps;
				}
				time = 0;
			}
			else
			{
				++time;
			}
		}
		//napms(1000);
		//++cookie;
		printCookie(windowCookie);
		printItem(windowItem);
		doupdate();
	}
	endwin();
}

void printCookie(WINDOW * window)
{
	//wborder(window, ' ', ACS_VLINE, ' ', ' ', ' ', ACS_VLINE, ' ', ACS_VLINE);
	wborder(window, ACS_VLINE, ACS_VLINE, ' ', ' ', ACS_VLINE, ACS_VLINE, ACS_VLINE, ACS_VLINE);
	char image[][33] =
	{
	"        AAAAAAAAAAAAAAAA        ",
	"      AAAAAAAAAAAAAAAAAAAA      ",
    "    AAAAAAAAAAAAAAAAAAAAAAAA    ",
	"  AAAAAAAAAAAAAAAAAAAAAAAAAAAA  ",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
    "AAAABBBBAAAAAAAAAAAAAAAAAAAAAAAA",
	"AABBBBBBBBAAAAAAAAAAAAAAAAAAAAAA",
	"AABBBBBBBBAAAAAAAAAAAAAAAAAAAAAA",
	"AAAABBBBAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAABBBBAAAAAAABBAAAAAAA",
	"  AAAAAAAAAABBBBAAAAAAAAAAAAAA  ",
	"    AAAAAAAAAAAAAAAAAAAAAAAA    ",
	"      AAAAAAAAAAAAAAAAAAAA      ",
	"        AAAAAAAAAAAAAAAA        "
	};
	for (int i = 0; i < 16; ++i)
	{
		for (int j = 0; j < 32; ++j)
		{
			if (image[i][j] == 'A')
			{
				mvwaddch(window, 6 + i, 10 + j, ACS_BLOCK);
			}
			else if (image[i][j] == 'B')
			{
				mvwaddch(window, 6 + i, 10 + j, ACS_CKBOARD);
			}
		}
	}
	mvwprintw(window, 3, 10, "0x%s [cookie]", cookie.str(0, std::ios_base::hex).c_str());

	cpp_int cps = 0;
	for (auto & e : item)
	{
		cps += e.number * e.cps;
	}
	mvwprintw(window, 4, 10, "0x%s [cps]", cps.str(0, std::ios_base::hex).c_str());
	wnoutrefresh(window);
}

void printItem(WINDOW * window)
{
	wborder(window, ACS_VLINE, ACS_VLINE, ' ', ' ', ACS_VLINE, ACS_VLINE, ACS_VLINE, ACS_VLINE);
	//wborder(window, ACS_VLINE, ' ', ' ', ' ', ACS_VLINE, ' ', ACS_VLINE, ' ');
	int y;
	int x;
	getbegyx(window, y, x);

	mvwprintw(window, 0, 3,  "Store");
	for (int i = 0; i < item.size(); ++i)
	{
		if (i == 3)
		{
			wattron(window, A_REVERSE);
		}
		mvwprintw(window, 2 + (i * 4), 2, "%s : 0x%s", item[i].name.c_str(), item[i].number.str(0, std::ios_base::hex).c_str());
		mvwprintw(window, 2 + (i * 4) + 1, 2, "0x%s [cps]", item[i].cps.str(0, std::ios_base::hex).c_str());
		mvwprintw(window, 2 + (i * 4) + 2, 2, "0x%s [cookie]", item[i].cost.str(0, std::ios_base::hex).c_str());
		wattroff(window, A_REVERSE);
	}
	wnoutrefresh(window);
}


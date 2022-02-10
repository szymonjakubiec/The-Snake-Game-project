#include<iostream>
#include<windows.h> //potrzebne do Sleep(),
					//do ustawiania cursora,
					//do CONSOLE_SCREEN_BUFFER_INFOEX i pozosta�ych funkcji zwi�zanych ze zmian� kolor�w,
					//do CONSOLE_SCREEN_BUFFER_INFO - ilo�ci kolumn i rz�d�w
#include<conio.h>//do _kbhit() i _getch()

using namespace std;

const int OY = 12, OX = 18;
char plansza[OY][OX] =
//     osie: OY  OX
{
	{"#################"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#################"} //10 x 15
};						  //150 p�l

//tu zapisuj� ruch podany przez gracza
char ruch;
char klawisz_g�ra{ 'w' }, klawisz_d�{ 's' }, klawisz_lewo{ 'a' }, klawisz_prawo{ 'd' };

enum Kierunki { lewo = 1, g�ra, d�, prawo };
Kierunki kierunek = prawo, poprzedni = prawo;


int w��X[150]; //tyle element�w tablicy ile mo�liwych p�l
int w��Y[150];

//wonszX[i] -> wsp�rz�dna X dla konkretnej "cz�ci" w�a 
//wonszX[0]	-> O -> g��wka w�a

//pocz�tkowa d�ugo�� czyli tylko g�owa
int d�{ 0 };

enum szybko�� { wolny = 1, �redni, szybki };
szybko�� szybko��_w�a = �redni, szybko��_poprzednia = wolny;

//--------------------------------------------------------------------
//koordynaty dla jab�ka
int xj, yj;

//czy w�� zjad� jab�ko?
bool am = false;

char � = 0x98, � = 0xa5, � = 0x86, � = 0xa9, � = 0xbe, � = 0x88, � = 0xab, � = 0xe4, � = 0xa2;
char lewa_strza�ka = 0x11, prawa_strza�ka = 0x10;

char naci�nij;

int czeka�; //do Sleepa

CONSOLE_SCREEN_BUFFER_INFO csbi;
int kolumny;

COORD c; //do zmieniania pozycji kursora

/*-------------------------------------------------------------------------------*/
/*									 FUNKCJE									 */
/*-------------------------------------------------------------------------------*/

void intro()
{
	//kolory
	CONSOLE_SCREEN_BUFFER_INFOEX info1;
	info1.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	
	HANDLE hConsole1 = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfoEx(hConsole1, &info1);

	info1.ColorTable[0] = RGB(0, 0, 0);//ten kolor jest t�em
	info1.ColorTable[1] = RGB(0, 135, 3); //do napisu SNAKE
	info1.ColorTable[2] = RGB(200, 200, 200);//powr�t do bia�ego

	SetConsoleScreenBufferInfoEx(hConsole1, &info1);
	//---------------------------------------------------------------
	do
	{
		//ile kolumn?
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		kolumny = csbi.srWindow.Right - csbi.srWindow.Left + 1; //srWindow zawiera koordynaty lewego g�rnego i prawego dolnego rogu

		system("cls");
		cout << endl << endl;
		cout.width((kolumny / 2) - 12);
		SetConsoleTextAttribute(hConsole1, 1);
		cout << " THE" << endl << endl;

		SetConsoleTextAttribute(hConsole1, 1);
		cout.width((kolumny / 2) + 16);
		cout << "# # #  #   #    #    #  #  # # #" << endl;
		cout.width((kolumny / 2) + 16);
		cout << "#      ##  #  #   #  # #   #    " << endl;
		cout.width((kolumny / 2) + 16);
		cout << "# # #  # # #  # # #  ##    # #  " << endl;
		cout.width((kolumny / 2) + 16);
		cout << "    #  #  ##  #   #  # #   #    " << endl;
		cout.width((kolumny / 2) + 16);
		cout << "# # #  #   #  #   #  #  #  # # #" << endl << endl;

		cout.width((kolumny / 2) + 16);
		cout << "GAME " << endl << endl;
		
		SetConsoleTextAttribute(hConsole1, 2);
		cout.width((kolumny / 2) + 16);
		cout << "       by Szymon Jakubiec       " << endl << endl;

		cout.width((kolumny / 2) - 10);
		cout << "  naci" << � << "nij ENTER aby rozpocz" << � << � << endl << endl;

		naci�nij = _getch();
		
	} while (naci�nij != 13);
}

void opcje()
{
	do
	{		
		system("cls");
		cout << endl << endl;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		kolumny = csbi.srWindow.Right - csbi.srWindow.Left + 1; //srWindow zawiera koordynaty lewego g�rnego i prawego dolnego rogu

		cout.width((kolumny / 2) - 18);
		cout << "zmie" << � << " szybko" << � << � << " w" << � << � << "a u" << � << "ywaj" << � << "c klawiszy (a,d)" << endl;


		cout.width((kolumny / 2) - 9);
		cout << "i zatwierd" << � << " klawiszem ENTER by zagra" << � << endl << endl;
		

		cout << "\r";
		cout.width((kolumny / 2) - 7);

		if (szybko��_w�a == �redni)
		{
			cout << "szybko" << � << � << " w" << � << � << "a:   " << lewa_strza�ka << � << "redni" << prawa_strza�ka;
		}
		else if (szybko��_w�a == wolny)
		{
			cout << "szybko" << � << � << " w" << � << � << "a:   " << " wolny" << prawa_strza�ka << " ";
		}
		else if (szybko��_w�a == szybki)
		{
			cout << "szybko" << � << � << " w" << � << � << "a:   " << lewa_strza�ka << "szybki ";
		}
		
		
		naci�nij = _getch();
		if ((naci�nij == 'a') || (naci�nij == 'd') || (naci�nij == 13))
		{
			if (szybko��_w�a == �redni)
			{
				if (naci�nij == 'a')		szybko��_w�a = wolny;
				else if (naci�nij == 'd')	szybko��_w�a = szybki;
			}
			else if (szybko��_w�a == wolny)
			{
				if (naci�nij == 'd')	szybko��_w�a = �redni;
			}
			else if (szybko��_w�a == szybki)
			{
				if (naci�nij == 'a')	szybko��_w�a = �redni;
			}
		}
	} while (naci�nij != 13);
}


int main()
{
	/*----------------------------------------------------------------------------------------------*/
	/*											  WST�P 											*/
	/*----------------------------------------------------------------------------------------------*/

	intro();

	opcje();

	if (szybko��_w�a == wolny)				czeka� = 200;
	else if (szybko��_w�a == �redni)		czeka� = 150;
	else if (szybko��_w�a == szybki)		czeka� = 100;
	
	/*----------------------------------------------------------------------------------------------*/
	/*											 PRZED GR�											*/
	/*----------------------------------------------------------------------------------------------*/
			
	system("cls");

	//kolory
	CONSOLE_SCREEN_BUFFER_INFOEX info2;
	info2.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);

	HANDLE hConsole2 = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfoEx(hConsole2, &info2);

	info2.ColorTable[0] = RGB(128, 229, 122);	//t�o rozgrywki
	info2.ColorTable[1] = RGB(20, 92, 20);		//kolor krzak�w
	info2.ColorTable[2] = RGB(225, 126, 66);	//jab�ko
	info2.ColorTable[3] = RGB(125, 93, 44);		//w��

	SetConsoleScreenBufferInfoEx(hConsole2, &info2);

	//wypisanie planszy
	SetConsoleTextAttribute(hConsole2, 1); //w kolorze krzak�w

	for (int i = 0; i < OY; i++)
		cout << *(plansza + i) << endl;
	
	//wypisanie punkt�w
	SetConsoleTextAttribute(hConsole2, 3); //by odr�nia�y si� kolorem od krzak�w
	cout << "punty:\t" << d�;

	//ustawienie pierwszego jab�ka
	do
	{
		xj = rand() % (OX - 3) + 1; //(1,17)		
		yj = rand() % (OY - 2) + 1;//(1,8)

		SetConsoleTextAttribute(hConsole2, 1);
	} while (plansza[yj][xj] != ' ');

	plansza[yj][xj] = '+';

	c.X = xj;
	c.Y = yj;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	SetConsoleTextAttribute(hConsole2, 2);//kolor jab�ka
	cout << '+';

	//startowe ustawienie w�a
	w��X[0] = 5;
	w��Y[0] = 5;

	/*--------------------------------------------------------------------------------------------------*/
	/*												   GRA												*/
	/*--------------------------------------------------------------------------------------------------*/

	while (d� < 149)
	{
		/*-------------------------   wype�nienie planszy w�em   --------------------------*/
		
		//g�owa w�a ma dl=0
		plansza[w��Y[0]][w��X[0]] = 'O';

		c.X = w��X[0];
		c.Y = w��Y[0];
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		SetConsoleTextAttribute(hConsole2, 3);//kolor w�a
		cout << 'O';

		if (d� > 0)
		{
			c.X = w��X[1];
			c.Y = w��Y[1];
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			cout << 'o';
		}

		/*----------------------------------------------------------------------------------*/
		/*										  ruch										*/
		/*----------------------------------------------------------------------------------*/
		
		//sztucznie przerzucam wska�nik w miejsce ilo�ci punkt�w by nie przeszkadza� przy w�u
		if (d� > 9)
			c.X = 10;
		else if (d� > 99)
			c.X = 11;
		else
			c.X = 9;
		c.Y = OY;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		Sleep(czeka�);		

		//tu wykrywam czy jaki� klawisz zosta� wci�ni�ty
		if (_kbhit())
		{
			ruch = _getch();
			if ((poprzedni == lewo) || (poprzedni == prawo))
			{
				if (ruch == klawisz_g�ra)		kierunek = g�ra;
				else if (ruch == klawisz_d�)	kierunek = d�;
			}
			else if ((poprzedni == g�ra) || (poprzedni == d�))
			{
				if (ruch == klawisz_lewo)		kierunek = lewo;
				else if (ruch == klawisz_prawo)	kierunek = prawo;
			}
		}


		if (am == false)
		{
			//"koniec" w�a usuwa si� z ostatniego miejsca 
			plansza[w��Y[d�]][w��X[d�]] = ' ';

			c.X = w��X[d�];
			c.Y = w��Y[d�];
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			cout << ' ';

			for (int i = d�; i > 0; i--)
			{
				w��X[i] = w��X[i - 1];
				w��Y[i] = w��Y[i - 1];
			}
		}
		else //czyli jak zjad� jab�ko
		{
			am = false;

			//w�� wyd�u�a si� w miejscu, kt�re opuszcza dotychczasowy "koniec" w�a
			d�++;
			w��X[d�] = w��X[d� - 1];
			w��Y[d�] = w��Y[d� - 1];
			plansza[w��Y[d�]][w��X[d�]] = 'o';

			c.X = w��X[d�];
			c.Y = w��Y[d�];
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			SetConsoleTextAttribute(hConsole2, 3);//kolor w�a
			cout << 'o';

			//update wypisu ilo�ci punkt�w
			c.X = 8;
			c.Y = OY;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			cout << d�;

			for (int i = d� - 1; i > 0; i--)
			{
				w��X[i] = w��X[i - 1];
				w��Y[i] = w��Y[i - 1];
			}

			/*-------------------   ustawienie kolejnego jab�ka	  -------------------*/
			{
				do
				{
					xj = rand() % (OX - 3) + 1; //(1,17)		
					yj = rand() % (OY - 2) + 1;//(1,8)

				} while (plansza[yj][xj] != ' ');
				plansza[yj][xj] = '+';

				//wypis 
				c.X = xj;
				c.Y = yj;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
				SetConsoleTextAttribute(hConsole2, 2);//kolor jab�ka
				cout << '+';
			}
		}

		/*---------------------		 w kt�r� stron� g�owa w�a?		----------------------------*/
		if (kierunek == g�ra)
		{
			if (plansza[w��Y[0] - 1][w��X[0]] == ' ')
			{
				w��Y[0]--;
			}
			else if (plansza[w��Y[0] - 1][w��X[0]] == '+')
			{
				w��Y[0]--;
				am = true; //w�� zjada jab�ko
			}
			else break;
		}
		//---------------------------------------------------------------
		else if (kierunek == d�)
		{
			if (plansza[w��Y[0] + 1][w��X[0]] == ' ')
			{
				w��Y[0]++;
			}
			else if (plansza[w��Y[0] + 1][w��X[0]] == '+')
			{
				w��Y[0]++;
				am = true;
			}
			else break;
		}
		//---------------------------------------------------------------
		else if (kierunek == lewo)
		{
			if (plansza[w��Y[0]][w��X[0] - 1] == ' ')
			{
				w��X[0]--;
			}
			else if (plansza[w��Y[0]][w��X[0] - 1] == '+')
			{
				w��X[0]--;
				am = true;
			}
			else break;
		}
		//---------------------------------------------------------------
		else if (kierunek == prawo)
		{
			if (plansza[w��Y[0]][w��X[0] + 1] == ' ')
			{
				w��X[0]++;
			}
			else if (plansza[w��Y[0]][w��X[0] + 1] == '+')
			{
				w��X[0]++;
				am = true;
			}
			else break;
		}

		poprzedni = kierunek;
	}

	/*----------------------------------------------------------------------------------*/
	/*									 KONIEC GRY										*/
	/*----------------------------------------------------------------------------------*/
	system("cls");

	Sleep(250);

	SetConsoleTextAttribute(hConsole2, 2);
	cout.width(24);
	if (d� < 149)
		cout << "KONIEC GRY!!!" << endl << endl;//24
	else
		cout << "GRATULACJE!!!" << endl << endl;
	SetConsoleTextAttribute(hConsole2, 3);
	cout << "     zdoby" << � << "e" << � << "(-a" << � << ") ";
	SetConsoleTextAttribute(hConsole2, 2);
	cout << d�;

	SetConsoleTextAttribute(hConsole2, 3);

	if (d� == 1)
		cout << " punkt";
	else if ((d� % 10 == 2) || (d� % 10 == 3) || (d� % 10 == 4))
	{
		if ((d� != 12) && (d� != 13) && (d� != 14))
			cout << " punkty";
		else
			cout << " punkt" << � << "w";
	}
	else
	{
		cout << " punkt" << � << "w";
	}
	cout << endl << endl;

	cout << "naci" << � << "nij dowolny klawisz aby wyj" << � << �;

	SetConsoleTextAttribute(hConsole2, 0);

	return 0;
}
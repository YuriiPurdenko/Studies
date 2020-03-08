#include"stdio.h"
#include"string.h"

//Wiktor Pilarczyk 308533 lista 2 zad 1
void zwdu()
{
	int zm;
	while(1)
	{
		zm=getchar();
		if(zm==EOF)
			break;
		if(zm<128)
			putchar(zm);
		else
		{
			if(zm==165)	//Ą
			{
				putchar(0xC4);
				putchar(0x84);		
			}
			if(zm==198)	//Ć
			{
				putchar(0xC4);
				putchar(0x86);		
			}
			if(zm==202)	//Ę
			{
				putchar(0xC4);
				putchar(0x98);		
			}
			if(zm==163)	//Ł
			{
				putchar(0xC5);
				putchar(0x81);		
			}
			if(zm==209)	//Ń
			{
				putchar(0xC5);
				putchar(0x83);		
			}
			if(zm==211)	//Ó
			{
				putchar(0xC3);
				putchar(0x93);		
			}
			if(zm==140)	//Ś
			{
				putchar(0xC5);
				putchar(0x9A);		
			}
			if(zm==143)	//Ź
			{
				putchar(0xC5);
				putchar(0xB9);		
			}
			if(zm==175)	//Ż
			{
				putchar(0xC5);
				putchar(0xBB);		
			}
			if(zm==185)	//ą
			{
				putchar(0xC4);
				putchar(0x85);		
			}
			if(zm==230)	//ć
			{
				putchar(0xC4);
				putchar(0x87);		
			}
			if(zm==234)	//ę
			{
				putchar(0xC4);
				putchar(0x99);		
			}
			if(zm==179)	//ł
			{
				putchar(0xC5);
				putchar(0x82);		
			}
			if(zm==241)	//ń
			{
				putchar(0xC5);
				putchar(0x84);		
			}
			if(zm==243)	//ó
			{
				putchar(0xC3);
				putchar(0xB3);		
			}
			if(zm==156)	//ś
			{
				putchar(0xC5);
				putchar(0x9B);		
			}
			if(zm==159)	//ź
			{
				putchar(0xC5);
				putchar(0xBA);		
			}
			if(zm==191)	//ż
			{
				putchar(0xC5);
				putchar(0xBC);		
			}
		}
	}
}
void zudw()
{
	int zm;
	int b;
	while(1)
	{
		zm=getchar();
		if(zm==EOF)
			break;
		if(zm==0xC4)
		{
			b=getchar();
			if(b==0x84)	//Ą
				putchar(165);
			else
			{
				if(b==0x86)	//Ć
					putchar(198);
				else
				{
					if(b==0x98)	//Ę
						putchar(202);
					else
					{	
						if(b==0x85)	//ą
							putchar(185);
						else
						{	
							if(b==0x87)	//ć
								putchar(230);
							else
							{	
								if(b==0x99)	//ę
									putchar(234);
							}
						}
					}
			
				}	
			}
		}
		else
		{
			if(zm==0xC5)
			{
				b=getchar();
				if(b==0x81) //Ł
					putchar(163);
				else
				{
					if(b==0x83) //Ń
						putchar(209);
					else
					{
						if(b==0x9A) //Ś
							putchar(140);
						else
						{
							if(b==0xB9) //Ź
								putchar(143);
							else
							{
								if(b==0xBB) //Ż
									putchar(175);
								else
								{
									if(b==0x82) //ł
										putchar(179);
									else
									{
										if(b==0x84) //ń
											putchar(241);
										else
										{
											if(b==0x9B) //ś
												putchar(156);
											else
											{
												if(b==0xBA) //ź
													putchar(159);
												else
												{
													if(b==0xBC) //ż
														putchar(191);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				if(zm==0xC3)
				{	
					b=getchar();
					if(b==0x93)//Ó
						putchar(211);
					else
					{
						if(b==0xB3)
							putchar(243);
					}
				}
				else
					putchar(zm);
			}
		}
	}
}
int main(int argc,char *argv[])
{
	if(argc<=1)
	{
		printf("Brak arumentow \n");
		return 1;
	}
	if(strcmp (argv[1],"win2utf")==0)
		zwdu();
	else
		zudw();
}

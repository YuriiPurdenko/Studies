
for /L %%i in (0,1,%1) DO (
    main.exe < in/%%i.in > mine/%%i.out
    python check.py %%i
)
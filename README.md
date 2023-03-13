# pi-clock
A simple program to display the current time using powers of π.

Made using Windows APIs because I can't be bothered to make a multiplatform shitpost.
<br/><br/>

By default it runs for 50 seconds, but you can change this by editing the number `50000` in line 154:

```c
time_t now = time(0);

    while (elapsed_time_ms < 50000) { <------- CHANGE THIS NUMBER
        elapsed_time_ms = (clock() - start_time) * 1000 / CLOCKS_PER_SEC; // Calculate elapsed time in milliseconds
```

Video of the code in action:

https://user-images.githubusercontent.com/53913349/224802409-f67b3c7c-8829-447b-9dab-8f543938aed0.mp4

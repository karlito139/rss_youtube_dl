Localtube
=========
Localtube is a youtube videos full bufferer. In other words if your got a crappy internet connection but still wants to see your videos in HD localtube will automatically download locally (in the folder of your choice) the videos of the youtube channels you subscribed to.

Plus it will keep up with the feed of videos, downloading them as soon as new ones are released.

For more informations, please visite the website : http://localtube.org/.


Compiling on windows
====================
Widows doesn't really know where to fins libssl. We need to install it from here : http://slproweb.com/products/Win32OpenSSL.html

Most of the time the right version is the "Win32 OpenSSL v1.0.2L", not the lattest one. This depend on the way QT was compiled. It can be tested with "QSslSocket::sslLibraryBuildVersionString();".

More informations here : https://stackoverflow.com/a/20357463/1696480
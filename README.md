# LYRA_Simple_Wallet

This is the nonofficial LYRA GUI ( Graphic User Interface ) very easy to use developed in QT for windows x64 and all distributions of Linux x64 base on Ubuntu.

The wallet will download and install all dependencies necessary to run ( .NET Core & LYRA CLI wallet ).

#### Usage:

Verify the integrity of the archive with Gpg4win downloaded from https://www.gpg4win.org ( or other pgp utility ) and extract the archive wherever you want.

Open the directory and:
* Double click on lyra-gui-wallet-win64-1.7.8.0.exe on windows.
* Open a terminal navigate to the wallet directory and type "./lyra-gui-wallet-ubuntu64-1.7.8.0-u1".

## Pay attention: GUI will never ask you to give administrator privileges on the machine, only installers need to.

There are only three components that need to be installed and ask you for administrator permission on windows:
* OpenSSL 1.1.1h Light (64-bit).
* Microsoft Visual C++ 2017 Redistributable (c64) v14.16.27033.
* .NET Core 3.1.

On linux will open the gnome-terminall and install .NET Core SDK and will ask you for administrator password, the script that install the dependencies are under "/dependency/" directory.

If they are already installed will not ask you to install them again.

Only if they are not already downloaded, will download:
* lyra.permissionless-1.7.8.0.tar.bz2. from official release download site.
* .NET Core 3.1. From official Microsoft site.

When all components are downloaded and extracted the GUI will open prompting to enter a wallet name if you already have one, if you do not have one, click cancel.

Now from menu you can create a wallet, recover one from private key, load an already existing one, send, receive LYR coins, the GUI does not synchronize automatically, the user need to click on sync button when necessary.

At this moment the GUI does not have a function to read and show the private key, if you want to read the private key you need to do it directly from CLI.

When you switch networks, if a wallet is opened at the moment of switch, the opened walled will be closed.

There is a debug window that can be opened at any time and see debug information and information's given by the CLI, the debug window will be erased only when you close the GUI interface.


### Donations kindly apreciated:
##### LYR: LCjM28ov1MciT8cd5TmSAiMgiLhSgXhYPUB6mWuqK3ZD8S5axLCtyihxkh5YZFpgbWML7WrC1d7sLLtaCbmT7YqX24ipZo

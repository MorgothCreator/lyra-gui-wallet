# LYRA_Simple_Wallet

This is the nonofficial LYRA GUI ( Graphic User Interface ) very easy to use developed in QT for windows x64 and all distributions of Linux x64 base on Ubuntu ( from Ubuntu 18.04 and up ).

The wallet will download and install all dependencies necessary to run ( .NET Core & LYRA CLI wallet ).

#### Usage:

Verify the integrity of the archive with Gpg4win downloaded from https://www.gpg4win.org ( or other pgp utility ) and extract the archive wherever you want.

Open the directory and:
* Double click on lyra-gui-wallet-win64-1.7.8.0.exe on windows.
* Open a terminal navigate to the wallet directory and type "./lyra-gui-wallet-ubuntu64-1.7.8.0-u1".

## Pay attention: GUI will never ask you to give administrator privileges on the machine, only installers need to.

#### On Windows:

Double click on "lyra-gui-wallet-win64-1.7.8.0.exe" executable.

There are only two components that need to be installed and ask you for administrator permission on windows:
* OpenSSL 1.1.1h Light (64-bit).
* Microsoft Visual C++ 2017 Redistributable (c64) v14.16.27033.

#### On Linux:

First time when you run the GUI wallet on a PC you need to open a terminal navigate to the wallet directory and type "./run_me_first_time.sh", the script will install "qt5-default" library, when it finishes will open the wallet to download .NET Core and LYRA CLI.

After first time you can open the wallet directly opening a terminal, navigating to the wallet place and write: "./lyra-gui-wallet-ubuntu64-1.7.8.0-u3".

#### On MacOS 10.13 and up:

There are issues in satisfying dependency's in order wallet to run ( If someone can fox this, please, you are welcome ), so there is a workaround installing Qt, downloading the <a href="https://www.qt.io/download-qt-installer?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4">QtOnlineInstaller.</a>, download it and fallow below instructions:

![LYRA GUY non official wallet](https://raw.githubusercontent.com/MorgothCreator/lyra-gui-wallet/master/QtOnMacOs.png)

#### On all platforms:

Only if they are not already downloaded, will download:
* lyra.permissionless-1.7.8.0.tar.bz2. from official release download site.
* .NET Core 3.1. binary package from official Microsoft site.

When all components are downloaded and extracted the GUI will open prompting to enter a wallet name if you already have one, if you do not have one, click cancel.

Now from menu you can create a wallet, recover one from private key, load an already existing one, send, receive LYR coins, the GUI does not synchronize automatically, the user need to click on sync button when necessary.

You can read private key, you can vote and it can show with who you voted for.

When you switch networks, if a wallet is opened at the moment of switch, the opened walled will be closed.

There is a debug window that can be opened at any time and see debug information and information's given by the CLI, the debug window will be erased only when you close the GUI interface.



![LYRA GUY non official wallet](https://raw.githubusercontent.com/MorgothCreator/lyra-gui-wallet/master/LYRA%20GUY%20non%20official%20wallet.png)


### Donations kindly apreciated:
##### LYR: LCjM28ov1MciT8cd5TmSAiMgiLhSgXhYPUB6mWuqK3ZD8S5axLCtyihxkh5YZFpgbWML7WrC1d7sLLtaCbmT7YqX24ipZo

# Padvish iLO Scanner
## Amnpardaz Software Co.

# What is this?

This is a scanner for HP iLO firmware. 

iLO is a management chip in HPE Servers which allows administration of the server. It is never turned off (even if the server itself is off), and can run code on a separate CPU with privileges far more than any OS.

For years, it was always thought as one of the best places for hiding malware, ... till a real rootkit was found. Unfortunately there is no way to scan or even "read" the firmware, so if you're infected, you'll never know.

Read more about the first iLO rootkit: [Implant.ARM.iLOBleed.a]

This tool was released by Amnpardaz. If you're curious, you can find more about [Amnpardaz] and [Padvish].

# Requirements

- **Booting with a Linux distro. (Any Linux distro will do)**
    The scanner needs direct HW access, so you have to use boot on bare-metal (ie. not a VM)
    Porting to another OS is possible but has not been done. Everyone is welcome to do that.
- **Root Privileges**
- **iLO Security Override Switch set to ON. (Located on the motherboard)**
    Yes, you have to physically open the server's case and turn the switch ON.
    Sorry, that was the only way we could dump the firmware so that the rootkit couldn't tamper with it. That's how the server was designed.
    Yes, you have to turn the switch back OFF when you're done, or you risk your server's security.

# How to Build

It's as easy as this:

```sh
$ gcc *.c -o PadvishILOScanner
```

Just download the code, and open the terminal in the source folder, and run the above command. (You'll need gcc installed, obviously)

# How to use

This is the help:

```
# ./PadvishILOScanner
Padvish iLO Scanner 1.0.0
(C) Amnpardaz Software Company 2022

Usage:
	PadvishILOScanner <Command>
	Commands:
		-i:	Find iLO in Hardware List
		-d:	Dump Firmware
```

You'd only need to run it with the (-d) command. **Just be sure you do it as "root", and that you've had turned the "iLO Security Switch" to ON state on the motherboard.**

If you're clean, you'll see something like this:

```
# ./PadvishILOScanner -d
...
Congratulations, you've got a VALID firmware. You're NOT infected. (HP iLO 4 - v2.79)
```

But if you see this, you'll have to beware:

```
# ./PadvishILOScanner -d
...
!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!! UNKNOWN FIRMWARE !!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!

Your iLO firmware was not found in our whitelist database.
This may indicate the server has an infected firmware.
Please contact apt@amnpardaz.com and provide the 'iLO_V4_1234567.bin' file to analyse the file
```

## How to turn on the iLO Security Switch

It's something like this, but you should refer to docs or search the Internet if you're unsure: ![](https://community.hpe.com/hpeb/attachments/hpeb/itrc-264/133839/1/IMG_maintenancesw1.jpg )

## I'm infected, what shoud I do?

If you see the `UNKNOWN FIRMWARE` message, you may be infected, or it may be the case that our whitelist is not comprehensive.
So you're advised to contact us (apt@amnpardaz.com) to investigate the case. 
If it is in fact an infection, you'll be provided help for fixing the issue, but there are other concerns you'll have to find out. (How and why you'd got infected)

# How can I prevent being infected?

- First of all, please take the time to investigate whether your servers are currently infected or not. It is the first thing you should do.
- On G10 servers, you should turn on the "Permanently Disallow Downgrades" policy. This option is available inside iLO’s configuration. You should consult the server’s documents.
- On G9 and older servers, unfortunately, there is not much protection you can put in place. It's always possible for the attacker to downgrade your firmware and infect it secretly. So you have to rely on mitigation methods to lower the risk of infection and periodically scan for the malware.

In either case, you can use the following tips to harden your servers and lower the risk:

- Do not connect the iLO network interface to the operating network and improvise a completely separate network. (Preferably using a separate switch, cabling, and management stations)
- Periodically update the iLO firmware version to the latest official release from the vendor
- Use defense-in-depth strategies to reduce risk and detect intrusions before reaching the iLO
- Periodically use the iLO Scanner tool to detect potential vulnerabilities, malware, and backdoors in the current version of the iLO Server firmware.

   [Implant.ARM.iLOBleed.a]: <https://threats.amnpardaz.com/en/2021/12/28/implant-arm-ilobleed-a/>
   [Amnpardaz]: <https://www.amnpardaz.com/en/>
   [Padvish]: <https://padvish.com/en-us/>



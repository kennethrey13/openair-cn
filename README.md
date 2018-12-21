# NOTICE:
This repo (https://github.com/uw-ictd/openair-cn.git) is a fork of the OpenAirInterface EPC (https://github.com/OPENAIRINTERFACE/openair-cn.git) is specifically intended to be a stable, long-term-support version of the code for use by people who simply want to get up and running with a basic LTE EPC. The EPC currently targets Release 14, which is the last of the LTE-Advanced releases before 5G; we have no current plans to support any other release.

## Which Branches Are Important And What Are The Changes?
On April 12, 2018, the CoLTE team forked OPENAIRINTERFACE/openair-cn and started stabilizing the most recent commit on the /develop branch, working from commit aad338 (fb_merge_y18w06). This repository does track all the changes made from OPENAIRINTERFACE/openair-cn onwards, but we specifically work ONLY in our own branches for purposes of clarity and ease of re-integration with the OpenAirInterface community and repository. The following two branches contain our changes:

### colte_stable (default branch):
To support CoLTE-specific services, we had to extend the OAI EPC with a couple of extra features and functions; these features are located only in colte_stable. If you are a small-scale network operator or ISP wanting to get started with CoLTE, this branch is for you.

### oai_stable:
This branch is designed to remain very similar to the standard OAI architecture, and contains nothing but code maturity/stability fixes. If you're a LTE engineer, just want a stable version of OAI, and already know how to operate it, this branch is for you.

## What is CoLTE?
CoLTE (https://github.com/uw-ictd/colte.git) is the Community LTE Project: an all-in-one software LTE network in a box that comes with a wide range of other services (i.e. a web-based admin tool, support for user monitoring/billing, etc).

## System Requirements
We currently support Ubuntu 18.04 (Bionic) and Debian 9 (Stretch).

## Quick Start: Use apt-get!
We host debian packages of precompiled binaries of the EPC that correspond to tagged releases on here. If you just want to get started with the EPC, copy-paste the following code:
```
echo "deb http://colte.cs.washington.edu $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/colte.list
sudo wget -O /etc/apt/trusted.gpg.d/colte.gpg http://colte.cs.washington.edu/keyring.gpg
sudo apt-get update
sudo apt-get install colte-epc
```

## Building the EPC
Building the EPC can be broken down into three main parts: build requirements, OAI-specific libraries (freeDiameter, asn1c, libgtpnl, and liblfds), and the EPC itself. To download all the build requirements, clone this repository and run `make build_deps` from the top level directory.

Once you have the build requirements, you need to install four OAI specific libraries before you can build the EPC. We provide these as pre-built debian packages in our repository; you can download them with the following commands:
```
echo "deb http://colte.cs.washington.edu $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/colte.list
sudo wget -O /etc/apt/trusted.gpg.d/colte.gpg http://colte.cs.washington.edu/keyring.gpg
sudo apt-get update
sudo apt-get install colte-freediameter colte-asn1c colte-libgtpnl colte-liblfds
```

Alternately, you can build and install the libraries yourself with the following commands:
```
make libraries
make libraries_deb
sudo dpkg -i BUILD/*.deb
```

Finally, you can build the three components of the EPC (HSS, MME, and SPGW) and package them into .deb files, along with the supporting packages `colte-epc` and `colte-db`, with `make all`. Just as above, the output packages will appear in the `BUILD` directory.

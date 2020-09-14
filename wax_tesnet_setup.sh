# open bash file
nano ~/.bashrc
# calling cleos -u .... as cleosw for WAX testnet
alias cleosw='cleos -u https://api.waxtest.greeneosio.com/'

# call source by executing
source ~/.bashrc

# check wax network
cleosw get info


# BeerYourself

Projeto de máquina para servir cerveja e chope em bares e festas, para a disciplina
de introdução ao projeto de sistemas embarcados da Unicamp (EA075).

A proposta é produzir não só um produto, mas um sistema que seja capaz de identificar
um cliente por meio de um cartão RFID, servir a bebida e registrar o consumo em sua
comanda digital.

## Requerimentos
 - Arduino IDE
 - Biblioteca [MFRC522](https://github.com/miguelbalboa/rfid)
 - Python3

## Execução
 - Compilar e atualizar o código src/main/main.ino em um Arduino UNO
 - Com o Arduino conectado na USB do computador, executar no terminal: `python3 src/beerserver.py` 
 - A API de python inicia com o modo de servir como padrão. Para adicionar um cliente pressione ***u***
 e para entrar em modo de manutenção, pressione ***m***

## Documentação
 - O diretório _doc/_ contém a apresentação, esquemático de conexão e [*link*](https://www.youtube.com/watch?v=p_S9DiA3JL4&feature=youtu.be) para o vídeo do equipamento
 funcionando no *YouTube*.

## Autores

 [Guilherme Domingues](https://github.com/guilhermeluisdomingues)
 [Henrique Roberto](https://github.com/Riquehr)
 [Marco Steck](https://github.com/steckmarco)
 [Rodrigo Caus](https://github.com/rodrigocaus)

import subprocess
def copy(python,imgfile):
    subprocess.check_call([python, "fool.pyz", imgfile,
"cp","article7.txt","Article7.txt",
"cp","article1.txt","aRticle1.txt",
"cp","article2.txt","arTicle2.txt",
"cp","article3.txt","artIcle3.txt",
"cp","article4.txt","artiCle4.txt",
"cp","article5.txt","articLe5.txt",
"cp","article6.txt","articlE6.txt",
"cp","const.txt","cOnst.txt",
"cp","article2.txt","my fancy filename.txt",
"cp","article3.txt","this is a really long filename.c",
"cp","billofrights.txt","BILL.TXT",
"cp","billofrights.txt","BILL.XA",
"cp","billofrights.txt","A338.TXT",
"cp","billofrights.txt","a338c def ghij klmn opq.txt",
"rm","ARTICLE5.TXT"
])

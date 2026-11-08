"""Install the officially signed MinGW x64 SDK without a system-wide change."""
from pathlib import Path
import base64,hashlib,importlib,json,shutil,subprocess,sys,tarfile,urllib.request
base=Path(__file__).resolve().parents[1]
deps=base/'Appeal_Sodium_Security/.deps';deps.mkdir(parents=True,exist_ok=True)
verify=deps/'verifier'
sys.path.insert(0,str(verify))
try: from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PublicKey
except ImportError:
    subprocess.run([sys.executable,'-m','pip','install','--disable-pip-version-check','--target',str(verify),'cryptography>=42,<50'],check=True)
    importlib.invalidate_caches()
    from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PublicKey
release='1.0.22-RELEASE'
metadata=json.load(urllib.request.urlopen(f'https://api.github.com/repos/jedisct1/libsodium/releases/tags/{release}',timeout=30))
public=base64.b64decode('RWQf6LRCGA9i53mlYecO4IzT51TGPpvWucNSCh1CBM0QTaLn73Y7GFO3')
manifest={'release':release,'verification':'Minisign Ed25519 signature; pinned upstream public key','assets':[]}
for name in ['libsodium-1.0.22-mingw.tar.gz','libsodium-1.0.22.tar.gz']:
    asset=next(a for a in metadata['assets'] if a['name']==name)
    target=deps/name
    with urllib.request.urlopen(asset['browser_download_url'],timeout=60) as response,target.open('wb') as out: shutil.copyfileobj(response,out)
    data=target.read_bytes();sha=hashlib.sha256(data).hexdigest()
    if asset.get('digest') and asset['digest']!='sha256:'+sha: raise RuntimeError('GitHub archive digest mismatch')
    signature=urllib.request.urlopen(asset['browser_download_url']+'.minisig',timeout=30).read()
    target.with_name(name+'.minisig').write_bytes(signature)
    packet=base64.b64decode(signature.decode().splitlines()[1])
    if packet[2:10]!=public[2:10] or packet[:2] not in (b'Ed',b'ED'): raise RuntimeError('Unexpected Minisign signing key or algorithm')
    signed=hashlib.blake2b(data,digest_size=64).digest() if packet[:2]==b'ED' else data
    Ed25519PublicKey.from_public_bytes(public[10:]).verify(packet[10:],signed)
    manifest['assets'].append({'name':name,'url':asset['browser_download_url'],'sha256':sha})
    output=deps/('sodium' if 'mingw' in name else 'source');output.mkdir(exist_ok=True)
    with tarfile.open(target) as archive:
        for member in archive.getmembers():
            if member.issym() or member.islnk() or not (member.isdir() or member.isfile()):continue
            final=(output/member.name).resolve()
            if not final.is_relative_to(output.resolve()): raise RuntimeError('Unsafe archive path')
            if member.isdir():final.mkdir(parents=True,exist_ok=True)
            else:
                final.parent.mkdir(parents=True,exist_ok=True)
                with archive.extractfile(member) as src,final.open('wb') as dst: shutil.copyfileobj(src,dst)
(deps/'SOURCE.json').write_text(json.dumps(manifest,indent=2))
print('Verified and installed libsodium SDK and matching source:',deps)

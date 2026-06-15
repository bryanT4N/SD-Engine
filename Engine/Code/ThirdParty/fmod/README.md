# FMOD (not included)

The engine's audio subsystem links against **FMOD**, which is proprietary middleware and is intentionally **not** committed to this public repository.

To build, install the **FMOD Engine SDK** (free for indie use — https://www.fmod.com/) and place its files here:

```
Engine/Code/ThirdParty/fmod/
  fmod.h  fmod.hpp  fmod_common.h  fmod_codec.h  fmod_dsp.h  fmod_dsp_effects.h
  fmod_errors.h  fmod_output.h
  fmod_vc.lib  fmod64_vc.lib
```

Then copy the runtime DLLs next to each game executable:

```
NetChess3D/Run/fmod.dll, fmod64.dll
AceAttorneyApproximation/Run/fmod.dll, fmod64.dll
```

These files are git-ignored so they are never committed back.

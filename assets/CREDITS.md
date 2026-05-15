# Asset Attribution

The visual experience on the AMOLED dashboard is heavily inspired by — and in
parts directly ported from — Hermann Bjorgvin's **Clawdmeter** project.

## Clawd pixel-art sprites
- **File:** [`src/clawd_animations.h`](../src/clawd_animations.h) — verbatim copy
- **Upstream source:** [github.com/HermannBjorgvin/Clawdmeter](https://github.com/HermannBjorgvin/Clawdmeter) (`firmware/src/splash_animations.h`)
- **Original art source:** [claudepix.vercel.app](https://claudepix.vercel.app) — 20×20 pixel-art creature animation library
- **Pipeline:** Clawdmeter's `tools/scrape_claudepix.js` + `tools/convert_to_c.js` produced the C arrays from the upstream HTML; we re-use the generated output as-is.

## Usage-rate classifier
- **Files:** [`src/usage_rate.h`](../src/usage_rate.h), [`src/usage_rate.cpp`](../src/usage_rate.cpp) — verbatim port
- **Upstream source:** [github.com/HermannBjorgvin/Clawdmeter](https://github.com/HermannBjorgvin/Clawdmeter) (`firmware/src/usage_rate.cpp/h`)
- **What it does:** Ring buffer of usage-percentage samples; classifies the
  recent growth rate into one of four groups (idle / normal / active / heavy)
  which drives Clawd's animation choice.

## Clawd renderer
- **Files:** [`src/clawd.h`](../src/clawd.h), [`src/clawd.cpp`](../src/clawd.cpp) — adapted, not verbatim
- **Upstream source:** [github.com/HermannBjorgvin/Clawdmeter](https://github.com/HermannBjorgvin/Clawdmeter) (`firmware/src/splash.cpp`)
- **Key changes:** Removed LVGL dependency and the dedicated canvas buffer.
  Renders directly to the framebuffer sprite via palette-resolved `uint16_t`
  writes. Preserved the group → animation list mapping, frame timing, and
  20-second auto-rotation cadence.

## Anthropic fonts
- **Files:** TBD (planned: `data/fonts/StyreneB-*.vlw`, `data/fonts/TiemposText-*.vlw`)
- **Source:** Anthropic-branded fonts (Styrene B Regular, Tiempos Text 400 Regular)
- **Note:** These fonts are Anthropic's property. They are included here on the
  authorization of the device owner. **The upstream maintainer of
  claude-usage-stick may need to remove them before merging** if a separate
  license is required for redistribution; the rest of this contribution
  functions without them (falls back to TFT_eSPI built-in fonts).

## Lucide status icons
- **Files:** TBD (planned: `src/lucide_icons.h`)
- **Upstream source:** [Lucide](https://lucide.dev) — MIT-licensed icon set;
  RGB565 arrays adapted from Clawdmeter's pre-converted `icons.h`.

## License notes
- The bulk of upstream Clawdmeter is **MIT-licensed** — see its `LICENSE` file.
  Their MIT terms permit copying with attribution, which this file provides.
- The claudepix sprite art has no explicit license at the source; we treat it
  as derivative-permissive based on Clawdmeter's prior incorporation.
- Anthropic fonts: see note above; redistribution authorization granted at the
  device-owner level.

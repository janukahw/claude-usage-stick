// Usage-rate tracker — classifies recent growth in the 5-hour utilization
// percentage into 4 groups (idle / normal / active / heavy). Driven by a small
// ring buffer of samples. Ported verbatim (data model is identical) from
// github.com/HermannBjorgvin/Clawdmeter (firmware/src/usage_rate.cpp/h).
#pragma once

void usage_rate_sample(float session_pct);
int  usage_rate_group(void);

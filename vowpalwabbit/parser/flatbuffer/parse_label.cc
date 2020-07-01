#include <fstream>
#include <iostream>
#include <cfloat>

#include "../../global_data.h"
#include "../../example.h"
#include "../../constant.h"
#include "../../cb.h"
#include "../../action_score.h"
#include "../../best_constant.h"
#include "parse_flat_example.h"
#include "generated/example_generated.h"


namespace VW {
namespace parsers {
namespace flatbuffer {

void parser::parse_simple_label(shared_data* sd, polylabel* l, const SimpleLabel* label)
{
  l->simple.label = label->label();
  l->simple.weight = label->weight();
  count_label(sd, label->label());
}

void parser::parse_cb_label(polylabel* l, const CBLabel* label)
{
  l->cb.weight = label->weight();
  for (auto const& cost : *(label->costs())){
    CB::cb_class f;
    f.action = cost->action();
    f.cost = cost->cost();
    f.probability = cost->probability();
    f.partial_prediction = cost->partial_pred();
    l->cb.costs.push_back(f);
    // No constructor available
    // l->cb.costs.emplace_back(cost->cost(), cost->action(), cost->probability(), cost->partial_pred());
  }
}

void parser::parse_ccb_label(polylabel* l, const CCBLabel* label)
{
  l->conditional_contextual_bandit.weight = label->weight();
  if (label->example_type() == 1) l->conditional_contextual_bandit.type = CCB::example_type::shared;
  else if (label->example_type() == 2) l->conditional_contextual_bandit.type = CCB::example_type::action;
  else if (label->example_type() == 3) {
    l->conditional_contextual_bandit.type = CCB::example_type::slot;

    if (label->explicit_included_actions() != nullptr){
      for (auto i=0;i<label->explicit_included_actions()->Length();i++){
        l->conditional_contextual_bandit.explicit_included_actions.push_back(label->explicit_included_actions()->Get(i));
      }
    }
    else if (label->outcome() != nullptr){
      auto& ccb_outcome = *(new CCB::conditional_contextual_bandit_outcome());
      ccb_outcome.cost = label->outcome()->cost();
      ccb_outcome.probabilities = v_init<ACTION_SCORE::action_score>();

      for (auto const& as : *(label->outcome()->probabilities()))
        ccb_outcome.probabilities.push_back({as->action(), as->score()});

      l->conditional_contextual_bandit.outcome = &ccb_outcome;
    }
  else l->conditional_contextual_bandit.type = CCB::example_type::unset;
    // l->conditional_contextual_bandit.outcome->cost = label->outcome()->cost();
  }
}

void parser::parse_cb_eval_label(polylabel* l, const CB_EVAL_Label* label)
{
  l->cb_eval.action = label->action();
  l->cb_eval.event.weight = label->event()->weight();
  for (auto i=0;i<label->event()->costs()->Length();i++){
    CB::cb_class f;
    f.cost = label->event()->costs()->Get(i)->cost();
    f.action = label->event()->costs()->Get(i)->action();
    f.probability = label->event()->costs()->Get(i)->probability();
    f.partial_prediction = label->event()->costs()->Get(i)->partial_pred();
    l->cb_eval.event.costs.push_back(f);
  }
}

void parser::parse_cs_label(polylabel* l, const CS_Label* label)
{
  for (auto i=0;i<label->costs()->Length();i++){
    COST_SENSITIVE::wclass f;
    f.x = label->costs()->Get(i)->x();
    f.partial_prediction = label->costs()->Get(i)->partial_pred();
    f.wap_value = label->costs()->Get(i)->wap_value();
    f.class_index = label->costs()->Get(i)->class_index();
    l->cs.costs.push_back(f); 
  }
}

void parser::parse_mc_label(shared_data* sd, polylabel* l, const MultiClass* label)
{
  uint32_t word = label->label();
  l->multi.label = sd->ldict ? (uint32_t)sd->ldict->get(std::to_string(word)) : word;
  l->multi.weight = label->weight();
}

void parser::parse_multi_label(polylabel* l, const MultiLabel* label)
{
  for (auto i=0;i<label->labels()->Length();i++)
    l->multilabels.label_v.push_back(label->labels()->Get(i));
}

void parser::parse_slates_label(polylabel* l, const Slates_Label* label)
{
  l->slates.weight = label->weight();
  if (label->example_type() == 1) {
    l->slates.labeled = label->labeled();
    l->slates.cost = label->cost();
  }
  else if (label->example_type() == 2) l->slates.slot_id = label->slot();
  else if (label->example_type() == 3) {
    l->slates.labeled = label->labeled();
    l->slates.probabilities = v_init<ACTION_SCORE::action_score>();

    for (auto const& as : *(label->probabilities()))
      l->slates.probabilities.push_back({as->action(), as->score()});    
  }
  else {
    THROW("Example type not understood")
  }
}

void parser::parse_no_label()
{
  // No Label
}
} // flatbuffer
} // parsers
} // VW
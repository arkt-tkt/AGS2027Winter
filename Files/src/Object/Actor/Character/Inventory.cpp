#include "../../../Manager/ItemManager.h"
#include "Player.h"
#include "Inventory.h"

Inventory::Inventory(Player* player)
	:
	player_(*player)
{
}

Inventory::~Inventory()
{
	Release();
}

void Inventory::Init()
{
	Add(SLOT::SLOT_PRIMARY1, "Weap_SG553");
	Add(SLOT::SLOT_PRIMARY2, "Weap_MP5K");
	Add(SLOT::SLOT_SECONDARY, "Weap_GL06");
	Add(SLOT::SLOT_BATTERY, "Weap_Scout");
	Add(SLOT::SLOT_AUXILIARY1, "Weap_MG3");

	Switch(SLOT::SLOT_PRIMARY1);
}

void Inventory::Update()
{
	for (auto& [slot, item] : inv_)
	{
		if (item != nullptr) item->Update();
	}

	const auto& INS = InputManager::GetInstance();
	const int INPUT_NUM = player_.GetInputNumber();

	if (INS.CheckDownMap(INPUT_NUM, InputManager::TAGS::WEAPON_SWITCH_F))
	{
		while (true)
		{
			SLOT nextSlot = static_cast<SLOT>
				((static_cast<int>(nowSlot_) + 1) % static_cast<int>(SLOT::SLOT_MAX));

			if (inv_[nextSlot] != nullptr)
			{
				Switch(nextSlot);
				break;
			}

			nowSlot_ = nextSlot;
		}
	}
	else if (INS.CheckDownMap(INPUT_NUM, InputManager::TAGS::WEAPON_SWITCH_R))
	{
		while (true)
		{
			SLOT nextSlot = static_cast<SLOT>
				((static_cast<int>(SLOT::SLOT_MAX) + static_cast<int>(nowSlot_) - 1) % static_cast<int>(SLOT::SLOT_MAX));

			if (inv_[nextSlot] != nullptr)
			{
				Switch(nextSlot);
				break;
			}

			nowSlot_ = nextSlot;
		}
	}

	isActed_ = false;

	if (INS.CheckDownMap(INPUT_NUM, InputManager::TAGS::RELOAD))
	{
		inv_[nowSlot_]->Reload();
	}

	if (INS.CheckNowMap(INPUT_NUM, InputManager::TAGS::ATTACK_SUB))
	{
		inv_[nowSlot_]->Sub();

		isActed_ = true;
	}

	if (INS.CheckDownMap(INPUT_NUM, InputManager::TAGS::ATTACK_CHANGE))
	{
		inv_[nowSlot_]->CycleFiremode();
	}

	isActed_ = inv_[nowSlot_]->Main(INPUT_NUM, InputManager::TAGS::ATTACK_MAIN) ? true : isActed_;
}

void Inventory::LateUpdate()
{
	inv_[nowSlot_]->LateUpdate();
}

void Inventory::Draw()
{
	inv_[nowSlot_]->Draw();
}

void Inventory::DrawUI()
{
	inv_[nowSlot_]->DrawUI();
}

void Inventory::Release()
{
	for (auto& [slot, item] : inv_)
	{
		if (item != nullptr) item->Release();
	}

	inv_.clear();
}

void Inventory::Switch(SLOT slot)
{
	if (slot == SLOT::SLOT_MAX) return;
	if (slot == nowSlot_) return;
	if (inv_[slot] == nullptr) return;
	if (inv_[slot]->IsFiring()) return;

	nowSlot_ = slot;
}

void Inventory::Add(SLOT slot, const std::string& name, bool replace)
{
	if (!replace)
	{
		auto it = inv_.find(slot);
		if (it != inv_.end()) return;
	}

	ItemBase::ItemData data = ItemManager::GetInstance().GetItemData(name);

	std::unique_ptr<ItemBase> newItem = std::make_unique<ItemBase>(data);
	newItem->SetParentTransform(player_.GetTransform());
	newItem->AddAwayColliders(sharedAwayCol_);
	newItem->Init();

	if (inv_[slot] != nullptr) inv_[slot]->Release();

	inv_[slot] = std::move(newItem);
}

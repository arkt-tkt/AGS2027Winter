#include "Player.h"
#include "Inventory.h"

Inventory::Inventory(Player* player)
	:
	player_(*player)
{
	Add(SLOT::SLOT_PRIMARY1);

	Switch(SLOT::SLOT_PRIMARY1);
}

Inventory::~Inventory()
{
	for (auto& [slot, item] : inv_)
	{
		if (item != nullptr) item->Release();
	}

	inv_.clear();
}

void Inventory::Update()
{
	for (auto& [slot, item] : inv_)
	{
		if (item != nullptr) item->Update();
	}

	const auto& INS = InputManager::GetInstance();
	const int INPUT_NUM = player_.GetInputNumber();

	if (INS.CheckDownMap(INPUT_NUM, InputManager::TAGS::RELOAD))
	{
		inv_[nowSlot_]->Reload();
	}

	if (INS.CheckNowMap(INPUT_NUM, InputManager::TAGS::ATTACK_SUB))
	{
		inv_[nowSlot_]->Sub();
	}

	if (INS.CheckDownMap(INPUT_NUM, InputManager::TAGS::CYCLE))
	{
		inv_[nowSlot_]->CycleFiremode();
	}

	inv_[nowSlot_]->Main(INPUT_NUM, InputManager::TAGS::ATTACK_MAIN);
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

void Inventory::Switch(SLOT slot)
{
	if (slot == SLOT::SLOT_MAX) return;
	if (slot == nowSlot_) return;
	if (inv_[slot] == nullptr) return;
	if (inv_[slot]->IsFiring()) return;

	nowSlot_ = slot;
}

void Inventory::Add(SLOT slot, bool replace)
{
	if (!replace)
	{
		auto it = inv_.find(slot);
		if (it != inv_.end()) return;
	}

	ItemBase::ItemData data;
	std::unique_ptr<ItemBase> newItem =
		std::make_unique<ItemBase>(data);
	newItem->SetParentTransform(player_.GetTransform());
	newItem->Init();

	if (inv_[slot] != nullptr) inv_[slot]->Release();

	inv_[slot] = std::move(newItem);
}

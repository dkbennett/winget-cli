// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "FontCommand.h"
#include "Workflows/CompletionFlow.h"
#include "Workflows/FontFlow.h"
#include "Workflows/WorkflowBase.h"
#include "Resources.h"

namespace AppInstaller::CLI
{
    using namespace AppInstaller::CLI::Execution;
    using namespace AppInstaller::CLI::Workflow;
    using namespace AppInstaller::Utility::literals;
    using namespace std::string_view_literals;

    Utility::LocIndView s_FontCommand_HelpLink = "https://aka.ms/winget-command-font"_liv;

    // Base Font Command

    std::vector<std::unique_ptr<Command>> FontCommand::GetCommands() const
    {
        return InitializeFromMoveOnly<std::vector<std::unique_ptr<Command>>>({
            std::make_unique<FontListCommand>(FullName()),
        });
    }

    Resource::LocString FontCommand::ShortDescription() const
    {
        return { Resource::String::FontCommandShortDescription };
    }

    Resource::LocString FontCommand::LongDescription() const
    {
        return { Resource::String::FontCommandLongDescription };
    }

    Utility::LocIndView FontCommand::HelpLink() const
    {
        return s_FontCommand_HelpLink;
    }

    void FontCommand::ExecuteInternal(Execution::Context& context) const
    {
        OutputHelp(context.Reporter);
    }

    // FontListCommand

    std::vector<Argument> FontListCommand::GetArguments() const
    {
        return {
            Argument::ForType(Args::Type::Family),
            Argument::ForType(Args::Type::Details),
        };
    }

    Resource::LocString FontListCommand::ShortDescription() const
    {
        return { Resource::String::FontListCommandShortDescription };
    }

    Resource::LocString FontListCommand::LongDescription() const
    {
        return { Resource::String::FontListCommandLongDescription };
    }

    void FontListCommand::Complete(Execution::Context& context, Args::Type valueType) const
    {
        UNREFERENCED_PARAMETER(valueType);
        context.Reporter.Error() << Resource::String::PendingWorkError << std::endl;
        THROW_HR(E_NOTIMPL);
    }

    Utility::LocIndView FontListCommand::HelpLink() const
    {
        return s_FontCommand_HelpLink;
    }

    void FontListCommand::ExecuteInternal(Execution::Context& context) const
    {
        context << Workflow::ReportInstalledFonts;
    }
}
